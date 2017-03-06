#include<iostream>
#include<vector>
#include<cstdint>
#include<thread>
#include<functional>
#include<math.h>
#include"config.hpp"
#include"output.hpp"
#include"../common/input.hpp"
#include"../common/math.hpp"

using namespace std;
void startTest(Config& config);
void saveStatistics(long SamplesPerTrace,int testOrder,
                    std::vector<float *> tStatistics,
                    std::vector<float *> pStatistics);
void ttestImpl(Input& input1,Input& input2,Config& config,
               float* mean1,float* variance1,
                float*mean2,float*variance2,
               float**trace1,float**trace2,
               int tau2,int tau3,int tau4,int tau5);
void onlineAlgorithmImpl(Input&input,Config&config,
                         float**trace,float*mean,float*variance,int traceNumber);
void setupTTest(Input&input,Config&config,float**trace,float**temp,
                float*mean,float*variance,
                int tau2,int tau3,int tau4,int tau5);
float computeQuantile(float alpha);
void computePValue(Input&i,float*t,float*p);
void prepareTrace(float**ts,float **temp,
		long tau,long tau3,long tau4,long tau5,long BATCH,long spt,int order);

std::vector<float *> tStatistics;			//to store list of t statistic (in case we compute for different taus)
std::vector<float *> pStatistics;			//to store list of p statistic (in case we compute for different taus)

int main(int argc,char* argv[]) {
    if(argc<2) {
        cout<<"Usage:\n./tTest.out configFile"<<endl;
        exit(0);
    }
    Config config(argv[1]);
    config.init();
    startTest(config);
}

void startTest(Config& config) {
    Input input1(config.filename);
    Input input2(config.filename2);
    input1.readHeader();
    input2.readHeader();
    int maxSample=(config.maxSample!=0 ? config.maxSample : input1.samplesPerTrace);
    if(input1.samplesPerTrace!=input2.samplesPerTrace) {
        cout<<"The traces should have the same sample number"<<endl;
        exit(0);
    }
    if(input1.numTraces%config.batch!=0) {
        cout<<"Step size should be a multiple of the number of traces"<<endl;
        exit(0);
    }
    if(maxSample>input1.samplesPerTrace) {
        cout<<"Wrong max sample"<<endl;
        exit(0);
    }
    /*
     * temp mean and temp variance for the two traces.
     * In case of an analysis on some samples only, 
     * all the possible space is allocated (some values
     * will remain 0 in that case).
     */
    cout<<endl<<"Starting t-test from sample "<<config.startSample<<" to sample "<<maxSample<<endl;
    float*mean1=new float[input1.samplesPerTrace];
    float*mean2=new float[input2.samplesPerTrace];
    float*variance1=new float[input1.samplesPerTrace];
    float*variance2=new float[input2.samplesPerTrace];
    float**trace1=new float*[config.batch];
    float**trace2=new float*[config.batch];
    for(int i=0;i<config.batch;i++)
        trace1[i]=new float[input1.samplesPerTrace];
    for(int i=0;i<config.batch;i++)
        trace2[i]=new float[input2.samplesPerTrace];
    switch(config.order) {
	case 1:
		ttestImpl(input1,input2,config,
                            mean1,variance1,mean2,variance2,
                            trace1,trace2,0,0,0,0);
		break;
	case 2:
		for(int tau=1;tau<=config.maxTau2;tau++)
                    ttestImpl(input1,input2,config,
                              mean1,variance1,mean2,
                              variance2,trace1,trace2,tau,0,0,0);
		break;
		//be careful, very computational intensive
	case 3:
		for(int tau=1;tau<=config.maxTau2;tau++) {
			for(int tau3=1;tau3<=config.maxTau3;tau3++) 
                            ttestImpl(input1,input2,config,
                                      mean1,variance1,mean2,variance2,
                                      trace1,trace2,tau,tau3,0,0);
		}
		break;
		//be careful, very computational intensive
	case 4:
		for(int tau=1;tau<=config.maxTau2;tau++) {
			for(int tau3=1;tau3<=config.maxTau3;tau3++) {
				for(int tau4=1;tau4<=config.maxTau4;tau4++)
                                    ttestImpl(input1,input2,config,
                                              mean1,variance1,mean2,variance2,
                                              trace1,trace2,tau,tau3,tau4,0);
			}
		}
		break;
	case 5:
		for(int tau=1;tau<=config.maxTau2;tau++) {
			for(int tau3=1;tau3<=config.maxTau3;tau3++) {
				for(int tau4=1;tau4<=config.maxTau4;tau4++) {
					for(int tau5=1;tau5<=config.maxTau5;tau5++)
                                            ttestImpl(input1,input2,config,
                                                      mean1,variance1,mean2,variance2,
                                                      trace1,trace2,tau,tau3,tau4,tau5);
				}
			}
		}
		break;
	}
    //at the end,save t values stored in the vectors
    Output output(config,input1,input2);
    output.writeResults(tStatistics,pStatistics);
}

void ttestImpl(Input& input1,Input& input2,Config& config,float* mean1,float* variance1,
                float*mean2,float*variance2,float**trace1,float**trace2,
               int tau2,int tau3,int tau4,int tau5) {    
    bool different=false;
    float*t=new float[input1.samplesPerTrace];
    //TODO:compute p values for each t
    float*p=new float[input1.samplesPerTrace];
    float** temp1;
    float** temp2;
    int maxSample=(config.maxSample!=0 ? config.maxSample : input1.samplesPerTrace);
    //if the test is of higher order, then allocate space in the heap
    if(config.order>1) {
        temp1=new float*[config.batch];
        temp2=new float*[config.batch];
        for(int i=0;i<config.batch;i++) {
            temp1[i]=new float[input1.samplesPerTrace];
            temp2[i]=new float[input2.samplesPerTrace];
        }
    }
    float quantile=computeQuantile(config.alpha);
    //init mean and variance
    for(int i=config.startSample;i<maxSample;i++) {
	mean1[i]=0;
	variance1[i]=0;
        mean2[i]=0;
	variance2[i]=0;
    }
    std::thread first(setupTTest,std::ref(input1),std::ref(config),trace1,temp1,
                mean1,variance1,tau2,tau3,tau4,tau5);
    std::thread second(setupTTest,std::ref(input2),std::ref(config),trace2,temp2,
                mean2,variance2,tau2,tau3,tau4,tau5);
    first.join();
    second.join();
    //here I have mean and variance for the taus specified (computed in the previous method setupTTest1,setupTTest2)
    for(int i=config.startSample;i<maxSample;i++) {
	variance1[i]/=(input1.numTraces-1);
	variance2[i]/=(input2.numTraces-1);
        //compute t statistic, for each point of the trace
	t[i]=(mean1[i]-mean2[i]) / sqrt(variance1[i]/input1.numTraces + variance2[i]/input2.numTraces);
	if(t[i]>quantile || t[i]<-quantile) different=true;
    }
    computePValue(input1,t,p);
    if(different) {
	cout<<"Null hypothesis refused for taus:"
            <<tau2<<" "<<tau3<<" "<<tau4<<" "<<tau5<<endl;
    } else {
	//confident to accept=(1-min p value)??
	cout<<"Null hypothesis can be accepted with a confident of "<<config.alpha<<endl;
    }
    //stores the t-values corresponding to the tau specified (there can be a lot of statistics, one for each tau)
    float *tValues=new float[input1.samplesPerTrace];
    float *pValues=new float[input1.samplesPerTrace];
    for(int i=0;i<input1.samplesPerTrace;i++) {
        tValues[i]=t[i];
        pValues[i]=p[i];
    }
    tStatistics.push_back(tValues);
    pStatistics.push_back(pValues);
}

void setupTTest(Input&input,Config&config,float**trace,float**temp,
                float*mean,float*variance,
                int tau2,int tau3,int tau4,int tau5) {
	int i=0;
        uint8_t**plains=new uint8_t*[config.batch];
        for(int j=0;j<config.batch;j++)
            plains[j]=new uint8_t[input.plainLength];
	while(i<input.numTraces) {
            input.readData(trace,plains,config.batch);
            //if and only if it is an higher order test, prepare the traces
            if(config.order>1) {
                prepareTrace(trace,temp,tau2,tau3,tau4,tau5,
                             config.batch,input.samplesPerTrace,config.order);
                onlineAlgorithmImpl(input,config,temp,mean,variance,i);
            } else
                onlineAlgorithmImpl(input,config,trace,mean,variance,i);
            i+=config.batch;
	}
}

void onlineAlgorithmImpl(Input&input,Config&config,
                         float**trace,float*mean,float*variance,int traceNumber) {
	float delta;
	float x;
        int count;
        int maxSample=(config.maxSample!=0 ? config.maxSample : input.samplesPerTrace);
        //about mean and variance array: only useful samples are modified
	for(int i=config.startSample;i<maxSample;i++) {
                count=traceNumber;
		for(int n=0;n<config.batch;n++) {
			count++;
			x=trace[n][i];
			delta=x-mean[i];
			mean[i]+=delta/count;
			variance[i]+=delta*(x-mean[i]);
		}
	}
}

/*
 * Prepare the traces putting in temp the new traces
 */
void prepareTrace(float**ts,float **temp,
		long tau,long tau3,long tau4,long tau5,long BATCH,long spt,int order) {
	//prepares the traces depending on the order
	switch(order) {
	case 2:
		for(int n=0;n<BATCH;n++) {
			for(int i=0;i<tau;i++) temp[n][i]=ts[n][i];
			for(int i=tau;i<spt;i++) {
				temp[n][i]=ts[n][i]-ts[n][i-tau];
			}
		}
		break;
	case 3:
		for(int n=0;n<BATCH;n++) {
			for(int i=0;i<tau3;i++) temp[n][i]=ts[n][i];
			for(int i=tau3;i<tau;i++) temp[n][i]=ts[n][i]-ts[n][i-tau3];
			for(int i=tau;i<spt;i++) {
				temp[n][i] =ts[n][i]-ts[n][i-tau]-ts[n][i-tau3];
			}
		}
		break;
	case 4:
		for(int n=0;n<BATCH;n++) {
			for(int i=0;i<tau4;i++) temp[n][i]=ts[n][i];
			for(int i=tau4;i<tau3;i++)temp[n][i]=ts[n][i]-ts[n][i-tau4];
			for(int i=tau3;i<tau;i++) temp[n][i]=ts[n][i]-ts[n][i-tau4]-ts[n][i-tau3];
			for(int i=tau;i<spt;i++) {
				temp[n][i]=ts[n][i]-ts[n][i-tau]-ts[n][i-tau3]-ts[n][i-tau4];
			}
		}
		break;
	case 5:
		for(int n=0;n<BATCH;n++) {
			for(int i=0;i<tau5;i++) temp[n][i]=ts[n][i];
			for(int i=0;i<tau4;i++) temp[n][i]=ts[n][i]-ts[n][i-tau5];;
			for(int i=tau4;i<tau3;i++)temp[n][i]=ts[n][i]-ts[n][i-tau4];
			for(int i=tau3;i<tau;i++) temp[n][i]=ts[n][i]-ts[n][i-tau4]-ts[n][i-tau3];
			for(int i=tau;i<spt;i++) {
				temp[n][i]=ts[n][i]-ts[n][i-tau]-ts[n][i-tau3]-ts[n][i-tau4];
			}
		}
		break;
	}

}

//compute the quantile with a given alpha
float computeQuantile(float alpha) {
    return -phi_inv(alpha/2);
}
void computePValue(Input& input,float*t,float*p) {
    for(int i=0;i<input.samplesPerTrace;i++) {
        p[i]=2*(1-phi(abs(t[i])));
    }
}
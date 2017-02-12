#include<iostream>
#include<cstdint>
#include<string>
#include <fstream>
#include"../common/input.hpp"
#include"../common/output.hpp"
#include"config.hpp"
#include"realigner.hpp"
#include"traceSplitter.hpp"

/**
 * Tool that alignes traces
 */
using namespace std;
int findIndexMax(float*array,int start,int end);
void showCorrelation(float*corr,Input& input);
int main(int argc,char*argv[]) {
    if(argc<2) {
        cout<<"Usage trace_aligner.out configFile."<<endl;
        exit(0);
    }
    Config config(argv[1]);
    config.init();
    Input input(config.filename);
    input.readHeader();
    if(input.numTraces%config.step!=0) {
        cout<<"Step size should be a multiple of the number of traces."<<endl;
        exit(0);
    }
    uint8_t**plain;
    if(config.m==multiple) {
        float** data=new float*[config.step];
        plain=new uint8_t*[config.step];
        for(int w=0;w<config.step;w++) {
            data[w]=new float[input.samplesPerTrace];
            plain[w]=new uint8_t[input.plainLength];
        }
        int i=0;
        //data matrix is modified by the Realigner
        Output output(config.outputFilename,config.step,input,data,plain);
        output.writeHeader();
        //first batch is outside to initialize the reference trace
        input.readData(data,plain,config.step);
        Realigner realigner(config,input,data[0]);
        cout<<"Aligning batch number "<<0<<endl;
        realigner.alignTraces(data);
        output.writeTraces();
        i+=config.step;
        while(i<input.numTraces) {
            input.readData(data,plain,config.step);
            cout<<"Aligning batch number "<<i/config.step<<endl;
            realigner.alignTraces(data);
            output.writeTraces();
            i+=config.step;
        }
    } else if(config.m==single) {
        float** data=new float*[1];
        input.samplesPerTrace=(config.endSample!=0 ? config.endSample : input.samplesPerTrace);
        data[0]=new float[input.samplesPerTrace];
        plain=new uint8_t*[1];
        plain[0]=new uint8_t[input.plainLength];
        cout<<"Reading data.."<<endl;
        input.readData(data,plain,1);
        Realigner realigner(config,input,data[0]);
        cout<<"Realigning the single trace.."<<endl;
        //initialize its correlation array
        float* correlation=new float[input.samplesPerTrace];
        realigner.autoCorrelate(correlation);
        if(config.printCorrelation)
            showCorrelation(correlation,input);
        cout<<"Splitting the trace.."<<endl;
        TraceSplitter traceSplitter(config,input);
        traceSplitter.splitTrace(correlation,data);
        cout<<"Splitting ended, traces saved"<<endl;
    }
    return 0;
}

int findIndexMax(float*array,int start,int end) {
    int indexMax=start;
    for(int i=start;i<end;i++) {
        if(array[i]>array[indexMax])
            indexMax=i;
    }
    return indexMax;
}

void showCorrelation(float*corr,Input& input) {  
    string datName="datCorr";
    string scriptName="correlation";
    std::ofstream outputScript,outputDat,outputPScript,outputPDat;
    
    outputScript.open(scriptName+".gpl");
    if(outputScript.fail()) {
        cerr << "Please provide a correct output script filename" << endl;
        exit(0);
    }
    outputDat.open(datName+".dat");
    if(outputDat.fail()) {
        cerr << "Please provide a correct output dat filename" << endl;
        exit(0);
    }
    for(int i=0;i<input.samplesPerTrace;i++) {
	outputDat<<i<<" ";
	outputDat<<corr[i]<<endl;
    }
    outputScript << "set term png size "<<1280<<","<<850<<endl;
    outputScript << "set output \""<< "correlation.png\";" << endl;
    outputScript << "set autoscale;" << endl;
    outputScript << "unset key;" << endl;
    outputScript << "set xlabel \"Sample\" font \",20\";" << endl;
    outputScript << "set ylabel \"t\" font \",20\";" << endl << endl;
    outputScript << "plot "<<datName<<".dat with lines"<<endl;
}
#include "statisticalAnalysis.hpp"
Pearson::Pearson(interval& i,Config& c,statistic* ts,statistic* ps,float** temp) {
    startSample=i.start;
    endSample=i.end;
    traceStatistics=ts;
    powerStatistics=ps;
    keySpace=pow(2,i.keySize);
    step=c.step;
    alpha=c.alpha;
    m12=temp;
    //init the matrix that will contain the "correct" values
    modifiedDataMatrix=new float*[step];
    for(int i=0;i<step;i++) 
        modifiedDataMatrix[i]=new float[endSample-startSample];   
}

void Pearson::computeDataStatistic(float** deltaTrace,int incrementalNumTraces) {
    int count,i,n;
    //update the intermediate values of mean and variance of the trace
    for(i=0;i<endSample-startSample;i++) {
        count=incrementalNumTraces-step;
        for(n=0;n<step;n++) {
            count++;
            deltaTrace[n][i]=(modifiedDataMatrix[n][i]-traceStatistics[i].mean);
            traceStatistics[i].mean+=deltaTrace[n][i]/count;
            traceStatistics[i].m2+=deltaTrace[n][i]*(modifiedDataMatrix[n][i]-traceStatistics[i].mean);
        }        
    }
}

void Pearson::computePowerStatistic(unsigned** powerMatrix,float** deltaPower,int incrementalNumTraces) {
    int count,i,n;
    //update the intermediate values of mean and variance of the power
    for(i=0;i<keySpace;i++) {
        count=incrementalNumTraces-step;
        for(n=0;n<step;n++) {
            count++;
            deltaPower[n][i]=(powerMatrix[n][i]-powerStatistics[i].mean);
            powerStatistics[i].mean+=deltaPower[n][i]/count;
            powerStatistics[i].m2+=deltaPower[n][i]*(powerMatrix[n][i]-powerStatistics[i].mean);
        }
    }
}
/*
 * Correlation computation
 */
void Pearson::correlate(int incrementalNumTraces,float** dataMatrix,unsigned**powerMatrix,float**pearsonCoefficient) {
    int i,n,count;
    float variancePower,varianceTrace;
    float** deltaPower=new float*[step];
    for(int w=0;w<step;w++)
        deltaPower[w]=new float[keySpace];
    float** deltaTrace=new float*[step];
    for(int w=0;w<step;w++)
        deltaTrace[w]=new float[endSample-startSample];
    //modifiedDataMatrix contains only useful samples
    this->generateDataMatrix(dataMatrix);
    //update the intermediate values of mean and variance of the trace
    thread t1(&Pearson::computeDataStatistic,this,deltaTrace,incrementalNumTraces);
    //update the intermediate values of mean and variance of the power
    thread t2(&Pearson::computePowerStatistic,this,powerMatrix,deltaPower,incrementalNumTraces);
    t1.join();
    t2.join();
   // computeDataStatistic(deltaTrace,incrementalNumTraces);
   // computePowerStatistic(powerMatrix,deltaPower,incrementalNumTraces);
    //update m12 and compute pearson
    for(i=0;i<keySpace;i++) {
        for(n=0;n<endSample-startSample;n++) {
            count=incrementalNumTraces-step;
            for(int s=0;s<step;s++) {
                count++;
                m12[i][n]+=((count-1)*(deltaPower[s][i]/count)*(deltaTrace[s][n]/count) - m12[i][n]/count);
            }
            variancePower=powerStatistics[i].m2/(count);
            varianceTrace=traceStatistics[n].m2/(count);
            pearsonCoefficient[i][n]=abs( m12[i][n]/(sqrt(variancePower)*sqrt(varianceTrace)) );
        }
    }
}
/*
 * slices the original matrix of traces, 
 * taking only the requested part of the time 
 */
void Pearson::generateDataMatrix(float** dataMatrix) {
    for(int i=0;i<step;i++) {
        for(int n=startSample;n<endSample;n++) {
            modifiedDataMatrix[i][n-startSample]=dataMatrix[i][n];
        }
    }
}
/*
 * if it is NOT a known key attack, search for the best pearson,
 * otherwise do not search for it because after this if branch
 * the key is already searched.
 * In case of a known key attack, the bestKey after this step 
 * will remain initialized to 0: this allows to have as second
 * best pearson coefficient 0.0, and so during confidence interval
 * computation the tool will generate the confidence intervals for
 * the correct key (the only one) and for 0, allowing to see if
 * the key is enough "far from" 0.
 */
void Pearson::findMax(float** matrix,result& r,int k,int row,int col) {
    //find the max pearson
    r.pearson=matrix[0][0];
    r.bestKey=0;
    r.pearson=0;
    if(keySpace>1) {
        for(int i=0;i<row;i++) {
            //do not check the correct key
            if(i==k) continue;
            for(int n=0;n<col;n++) {
                if(matrix[i][n]>r.pearson) {
                    r.bestKey=i;
                    r.pearson=matrix[i][n];
                }
            }
        }
    }
    //if it is a known key attack instead, the correct key is initialized to 0
    if(keySpace==1) k=0;
    //find the max pearson for the correct key
    r.pearsonCorrect=matrix[k][0];
    for(int i=0;i<col;i++) {
        if(matrix[k][i]>r.pearsonCorrect) 
            r.pearsonCorrect=matrix[k][i];
    }
}
/* 
 * in case of a known input attack,pearson variable will be 0,
 * while pearsonCorrect will be the only one correlation 
 * coefficient (preoduced by the known input). 
 */
void Pearson::computeConfidence(int incrementalNumTraces,result& r) {
    r.ic=this->getConfidence(r.pearson,incrementalNumTraces);
    r.icCorrect=this->getConfidence(r.pearsonCorrect,incrementalNumTraces);
}
/*
 * Confidence interval computation
 */
float Pearson::getConfidence(float r,int n) {
    float gamma=1-alpha;
    float fr=atanh(r);
    float zScore=sqrt(n-3)*fr;
    //obtain Gaussian quantile of transformed z_score
    float halfWidth=sqrt(2)*boost::math::erf_inv(gamma);
    //compute confidence interval in Fisher domain
    float inf=zScore-halfWidth;
    float sup=zScore+halfWidth;
    //invert Fisher map to get the domain straight again
    inf=tanh(inf/sqrt(n-3));
    sup=tanh(sup/sqrt(n-3));
    return sup-inf;
}
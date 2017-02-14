#include"realigner.hpp"

Realigner::Realigner(Config& config,Input& input,float*ref) {
    step=config.step;
    maxTau=config.maxTau;
    samplesPerTrace=input.samplesPerTrace;
    startSample=config.startSample;
    endSample=(config.endSample!=0 ? config.endSample : input.samplesPerTrace);
    function=config.function;
    refTrace=new float[input.samplesPerTrace];
    //store the very first trace
    for(int w=0;w<input.samplesPerTrace;w++)
        refTrace[w]=ref[w];
    //TODO:remove these attributes if not used
    samplingFreq=config.samplingFreq;
    cipherTime=config.cipherTime;
}

void Realigner::alignTraces(float** trace) {
    int tau,finalTau=0;
    //inserted here and not inside the for loop for performance reasons
    switch(function) {
        case crossCorr:
            for(int i=1;i<step;i++) {
                float index=0,currentIndex;
                for(tau=-maxTau;tau<=maxTau;tau++) {
                    currentIndex=crossCorrelate(refTrace,
                                                trace[i],startSample,endSample,tau,samplesPerTrace);
                    if(currentIndex>index || (currentIndex>=index && abs(tau)<abs(finalTau))) {
                        index=currentIndex;
                        finalTau=tau;
                    }
                }
                cout<<"Final tau found:"<<finalTau<<endl;
                shiftTrace(trace[i],trace[i],finalTau);
            }
            break;
        //here other function types
    }    
}

void Realigner::shiftTrace(float* trace,float* shiftedTrace,int tau) {
    int i,n;
    if(tau<0) {
        n=0;
        tau=abs(tau);
        for(i=tau;i<samplesPerTrace;i++) {
            shiftedTrace[n]=trace[i];
            n++;
        }
        for(;n<samplesPerTrace;n++)
            shiftedTrace[n]=0;
    } else {
        n=samplesPerTrace;
        for(i=samplesPerTrace-tau;i>=0;i--) {
            shiftedTrace[n]=trace[i];
            n--;
        }
        for(;n>=0;n--)
            shiftedTrace[n]=0;
    }
}

void Realigner::autoCorrelate(float* correlation) {
    for(int tau=0;tau<endSample-startSample;tau++) {
        correlation[tau]=crossCorrelate(refTrace,refTrace,startSample,endSample,-tau,samplesPerTrace);
    }
}
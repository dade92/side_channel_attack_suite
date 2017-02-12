#include"realigner.hpp"

Realigner::Realigner(Config& config,Input& input,float*ref) {
    step=config.step;
    maxTau=config.maxTau;
    samplesPerTrace=input.samplesPerTrace;
    startSample=config.startSample;
    endSample=(config.endSample!=0 ? config.endSample : input.samplesPerTrace);
    function=config.function;
    shiftedTrace=new float[input.samplesPerTrace];
    refTrace=new float[input.samplesPerTrace];
    //store the very first trace
    for(int w=0;w<input.samplesPerTrace;w++)
        refTrace[w]=ref[w];
    //TODO:remove these attributes if not used
    samplingFreq=config.samplingFreq;
    cipherTime=config.cipherTime;
}

void Realigner::alignTraces(float** trace) {
    int tau,finalTau;
    //inserted here and not inside the for loop for performance reasons
    switch(function) {
        case crossCorr:
            for(int i=1;i<step;i++) {
                float index=0,currentIndex;
                for(tau=-maxTau;tau<=maxTau;tau++) {
                    shiftTrace(trace[i],shiftedTrace,tau);
                    currentIndex=crossCorrelate(refTrace,
                                                shiftedTrace,startSample,endSample);
                    if(currentIndex>index) {
                        index=currentIndex;
                        finalTau=tau;
                    }
                }
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
    for(int w=0;w<samplesPerTrace;w++) {
        shiftTrace(refTrace,shiftedTrace,-w);
        correlation[w]=crossCorrelate(refTrace,shiftedTrace,startSample,endSample);
    }
}
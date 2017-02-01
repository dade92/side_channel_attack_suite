#ifndef TRANSFORM_H_
#define TRANSFORM_H_
#include"transform.hpp"

Transform::Transform(Config& config,Input& input,int ts,float** data) {
    windows=config.windows;
    dataMatrix=data;
    pad=config.pad;
    traceSize=ts;
    step=config.step;
    samplesPerTrace=input.samplesPerTrace;
    transformation=fftwf_alloc_complex(input.samplesPerTrace);
    filter=fftwf_alloc_complex(input.samplesPerTrace);
}

void Transform::padTraces() {
    switch(pad) {
        case zero:
            for(int i=0;i<step;i++) {
                for(int n=samplesPerTrace;n<traceSize;n++)
                    dataMatrix[i][n]=0;
            }
        case hold:
            for(int i=0;i<step;i++) {
                for(int n=samplesPerTrace;n<traceSize;n++)
                    dataMatrix[i][n]=dataMatrix[i][samplesPerTrace-1];
            }
    }
}

void Transform::computeFilter() {
    //generate filter window
    vector<window>::iterator it;
    //for each window, compute the filter specified by the user
    for(it=windows.begin();it!=windows.end();++it) {
        
    }
}

void Transform::filterTraces() {
    
}
#endif
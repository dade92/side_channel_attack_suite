#include<iostream>
#include<fftw3.h>
#include<cstdint>
#include<stdio.h>
#include"filter_generator_config.hpp"
#include"../common/input.hpp"
#include"../common/math.hpp"

using namespace std;

int main(int argc,char*argv[]) {
    if(argc<2) {
        cout<<"Usage ./adaptive_filter_generator.out configFile"<<endl;
        exit(0);
    }
    Config config(argv[1]);
    config.init();
    Input input(config.filename);
    input.readHeader();
    //used for padding
    uint32_t traceSize=(input.samplesPerTrace);
    fftwf_plan plan;
    FILE*fp; 
    float delta;
    float** trace=new float*[input.numTraces];
    uint8_t** plain=new uint8_t*[input.numTraces];
    //where to put the filter
    fftwf_complex* filterFunction=fftwf_alloc_complex(traceSize);
    fftwf_complex* signal=fftwf_alloc_complex(traceSize);
    for(int w=0;w<input.numTraces;w++) {
        trace[w]=new float[input.samplesPerTrace];
        plain[w]=new uint8_t[input.plainLength];
    }
    input.readData(trace,plain,1);
    for(int i=0;i<traceSize;i++) {
        signal[i][0]=trace[0][i];
        signal[i][1]=0;
    }
    /*
    for(int w=0;w<input.samplesPerTrace;w++)
        average[w]=0;
    int n=0;
    int count=0;
    float x;
    //TODO:is it correct to compute the mean? or it is better to take a single AES trace?
    cout<<"File loaded, computing mean.."<<endl;
    while(n<=input.numTraces) {
        input.readData(trace,plain,config.step);
        n+=config.step;
        //for every sample
        for(int i=0;i<input.samplesPerTrace;i++) {
            count=n-config.step;
            for(int j=0;j<config.step;j++) {
                count++;
                x=trace[j][i];
                delta=x-average[i];
                average[i]+=delta/count;
            }
        }
    }
    //padding to a power of 2 (DFT is much faster)
    switch(config.pad) {
        case zero:
            for(int n=input.samplesPerTrace;n<traceSize;n++)
                average[n]=0;
            break;
        case hold:
            for(int n=input.samplesPerTrace;n<traceSize;n++)
                average[n]=average[input.samplesPerTrace-1];
            break;
        case mean:
            float m;
            for(int i=0;i<config.step;i++) {
                m=computeMean(average,input.samplesPerTrace);
                for(int n=input.samplesPerTrace;n<traceSize;n++)
                    average[n]=m;
            }
            break;
    }
    cout<<"Mean computed, transforming to frequency domain.."<<endl;*/
    plan=fftwf_plan_dft_1d(traceSize,signal,filterFunction,FFTW_FORWARD,FFTW_ESTIMATE);
    //transformation
    fftwf_execute(plan);
    //normalize the filter
    //TODO:find out how to normalize
    float modulus;
    for(int i=0;i<traceSize;i++) {
        modulus=sqrt(pow(filterFunction[i][0],2)+pow(filterFunction[i][1],2));
        filterFunction[i][0]/=modulus;
        filterFunction[i][1]/=modulus;
    }
    //write the trace, with header
    cout<<"Writing to file.."<<endl;
    fp=fopen(config.outputFilename.c_str(),"wb");
    uint32_t numTraces=1;
    char sampleType='c';
    uint8_t plainLength=0;
    fwrite(&numTraces, sizeof(uint32_t), 1, fp);
    fwrite(&traceSize, sizeof(uint32_t), 1, fp);
    fwrite(&sampleType,sizeof(char),1,fp);
    fwrite(&plainLength, sizeof(uint8_t), 1, fp);
    //write the mean
    fwrite(filterFunction,sizeof(fftwf_complex),traceSize,fp);
    fftwf_destroy_plan(plan);
    return 0;    
}
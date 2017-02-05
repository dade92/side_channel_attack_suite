#include"transform.hpp"

Transform::Transform(Config& config,Input& input,int ts,float** data) {
    windows=config.windows;
    dataMatrix=data;
    pad=config.pad;
    traceSize=ts;
    samplingFreq=config.samplingFreq;
    step=config.step;
    samplesPerTrace=input.samplesPerTrace;
    fc=config.filterComb;
    //how many samples do I have to allocate?
    transformation=fftwf_alloc_complex(traceSize);
    filterFunction=fftwf_alloc_complex(traceSize);
    for(int i=0;i<traceSize;i++) {
        filterFunction[i][0]=0;
        filterFunction[i][1]=0;
    }
}

void Transform::padTraces() {
    switch(pad) {
        case zero:
            for(int i=0;i<step;i++) {
                for(int n=samplesPerTrace;n<traceSize;n++)
                    dataMatrix[i][n]=0;
            }
            break;
        case hold:
            for(int i=0;i<step;i++) {
                for(int n=samplesPerTrace;n<traceSize;n++)
                    dataMatrix[i][n]=dataMatrix[i][samplesPerTrace-1];
            }
            break;
        case mean:
            float mean;
            for(int i=0;i<step;i++) {
                mean=computeMean(dataMatrix[i],samplesPerTrace);
                for(int n=samplesPerTrace;n<traceSize;n++)
                    dataMatrix[i][n]=mean;
            }
            break;
    }
}

void Transform::computeFilter() {
    int i,k,N,n;
    vector<window>::iterator it;
    int freqIndexLow,freqIndexHigh;
    //for each window, compute the filter specified by the user
    for(it=windows.begin();it!=windows.end();++it) {
        //init the windows index (k:N=f:F)
        freqIndexLow=it->lowFrequency*traceSize/samplingFreq;;
        freqIndexHigh=it->highFrequency*traceSize/samplingFreq;
        N=freqIndexHigh-freqIndexLow;
        //TODO:manage even or odd window
        switch(it->windowFunction) {
            case rect:
                switch(it->type) {
                    case lowPass:
                        for(k=0;k<freqIndexHigh;k++)
                            filterFunction[k][0]+=1;
                        for(k=traceSize-freqIndexHigh;k<traceSize;k++)
                            filterFunction[k][0]+=1;
                        break;
                    case bandPass:
                        for(k=0;k<traceSize/2;k++) {
                            if(k>=freqIndexLow && k<=freqIndexHigh)
                                filterFunction[k][0]+=1;
                        }
                        for(k=traceSize/2;k<traceSize;k++) {
                            if(k>=traceSize-freqIndexHigh && k<=traceSize-freqIndexLow)
                                filterFunction[k][0]+=1;
                        }                        
                        break;
                    case highPass:
                        for(k=freqIndexLow;k<traceSize/2+freqIndexLow;k++)
                            filterFunction[k][0]+=1;
                        break;
                }
                break;
            case hann:
                switch(it->type) {
                    case lowPass:
                        for(k=0;k<freqIndexHigh;k++) {
                            n=k+N/2;
                            filterFunction[k][0]+=hanning(n,N);
                        }
                        for(k=traceSize-freqIndexHigh;k<traceSize;k++) {
                            n=k-(traceSize-freqIndexHigh);
                            filterFunction[k][0]+=hanning(n,N);
                        }
                        break;
                    case bandPass:
                        for(k=0;k<traceSize/2;k++) {
                            if(k>=freqIndexLow && k<=freqIndexHigh) {
                                n=k-freqIndexLow;
                                filterFunction[k][0]+=hanning(n,N);
                            }
                        }
                        for(;k<traceSize;k++) {
                            if(k>=traceSize-freqIndexHigh && k<=traceSize-freqIndexLow) {
                                n=k-(traceSize-freqIndexHigh);
                                filterFunction[k][0]+=hanning(n,N);
                            }
                        }
                        break;
                    case highPass:
                        for(k=freqIndexLow;k<traceSize/2+freqIndexLow;k++) {
                            n=k-freqIndexLow;
                            filterFunction[k][0]+=hanning(n,N);
                        }
                        break;
                }
                break;
            case nuttall:
                float a0=0.355768;
                float a1=0.487396;
                float a2=0.144232;
                float a3=0.012604;
                switch(it->type) {
                    case lowPass:
                        for(k=0;k<freqIndexHigh;k++) {
                            n=k+N/2;
                            filterFunction[k][0]+=blackman_nuttall(a0,a1,a2,a3,n,N);
                        }
                        for(k=traceSize-freqIndexHigh;k<traceSize;k++) {
                            n=k-(traceSize-freqIndexHigh);
                            filterFunction[k][0]+=blackman_nuttall(a0,a1,a2,a3,n,N);
                        }
                        break;
                    case bandPass:
                        for(k=0;k<traceSize/2;k++) {
                            if(k>=freqIndexLow && k<=freqIndexHigh) {
                                n=k-freqIndexLow;
                                    filterFunction[k][0]+=blackman_nuttall(a0,a1,a2,a3,n,N);
                            }
                        }
                        for(k=traceSize/2;k<traceSize;k++) {
                            if(k>=traceSize-freqIndexHigh && k<=traceSize-freqIndexLow) {
                                n=k-(traceSize-freqIndexHigh);
                                filterFunction[k][0]+=blackman_nuttall(a0,a1,a2,a3,n,N);
                            }
                        }
                        break;
                    case highPass:
                    for(k=freqIndexLow;k<traceSize/2+freqIndexLow;k++) {
                            n=k-freqIndexLow;
                            filterFunction[k][0]+=blackman_nuttall(a0,a1,a2,a3,n,N);
                        }                        
                        break;
                }
                break;
                //TODO:add here other windows
        }
    }
    //apply the filter combining policy
    switch(fc) {
        case doNothing: //only for completeness
            break;
        case clamp:
            for(k=0;k<traceSize;k++) {
                if(filterFunction[k][0]>1)
                    filterFunction[k][0]=1;
            }
            break;
        /*case normalize:
            float max=findMax(filterFunction,traceSize);
            for(k=0;k<traceSize;k++)
                filterFunction[k][0]/=max;
            break;*/
    }
}

void Transform::computeFilter(string inputTrace) {
    FILE* fp=fopen(inputTrace.c_str(),"rb");
    if(fp==NULL) {
        cout<<"Can't open filter."<<endl;
        exit(0);
    }
    int numSamples;
    //gets the number of bins of the filter
    fread(&numSamples,sizeof(int),1,fp);
    cout<<"Num samples:"<<numSamples<<endl;
    if(numSamples!=traceSize) {
        cout<<"Different size of filter and traces."<<endl;
        exit(0);
    }
    //get  the filter from file
    fread(filterFunction,sizeof(fftwf_complex),numSamples,fp);
}

void Transform::filterTraces() {
    fftwf_plan plan,anti_plan;
    int i,n;
    for(i=0;i<step;i++) {
        plan=fftwf_plan_dft_r2c_1d(traceSize,dataMatrix[i],transformation,FFTW_ESTIMATE);
        fftwf_execute(plan);
        for(n=0;n<traceSize;n++) {
            transformation[n][0]=transformation[n][0]*filterFunction[n][0] - 
                transformation[n][1]*filterFunction[n][1];
            transformation[n][1]=transformation[n][0]*filterFunction[n][1]+
                transformation[n][1]*filterFunction[n][0];
        }
        fftwf_destroy_plan(plan);
        anti_plan=fftwf_plan_dft_c2r_1d(traceSize,transformation,dataMatrix[i],FFTW_ESTIMATE);
        fftwf_execute(anti_plan);
        fftwf_destroy_plan(anti_plan);
    }
    //normalize the transformation
    for(i=0;i<step;i++) {
        for(n=0;n<traceSize;n++)
            dataMatrix[i][n]/=traceSize;
    }
}
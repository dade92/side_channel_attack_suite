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
    tempFilter=fftwf_alloc_complex(traceSize);
    for(int i=0;i<traceSize;i++) {
        filterFunction[i][0]=0;
        filterFunction[i][1]=0;
        tempFilter[i][0]=0;
        tempFilter[i][1]=0;
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
    int i,k,N;
    vector<window>::iterator it;
    int freqIndexLow,freqIndexHigh;
    //for each window, compute the filter specified by the user
    for(it=windows.begin();it!=windows.end();++it) {
        //init the windows index (k:N=f:F)
        freqIndexLow=it->lowFrequency*traceSize/samplingFreq;;
        freqIndexHigh=it->highFrequency*traceSize/samplingFreq;
        switch(it->windowFunction) {
            case rect:
                switch(it->type) {
                    case lowPass:
                        //TODO:manage even or odd window
                        for(k=0;k<freqIndexHigh;k++)
                            tempFilter[k][0]=1;
                        for(traceSize-freqIndexHigh;k<traceSize;k++)
                            tempFilter[k][0]=1;
                        break;
                    case bandPass:
                        for(k=0;k<traceSize/2;k++) {
                            if(k>=freqIndexLow && k<=freqIndexHigh)
                                tempFilter[k][0]=1;
                        }
                        for(k=traceSize/2;k<traceSize;k++) {
                            if(k>=traceSize-freqIndexHigh && k<=traceSize-freqIndexLow)
                                tempFilter[k][0]=1;
                        }                        
                        break;
                    case highPass:
                        for(k=freqIndexLow;k<traceSize/2+freqIndexLow;k++)
                            tempFilter[k][0]=1;
                        break;
                }
                break;
            case hann:
                N=freqIndexHigh-freqIndexLow;
                int n;
                switch(it->type) {
                    case lowPass:
                        for(k=0;k<freqIndexHigh;k++) {
                            n=k+N/2;
                            tempFilter[k][0]=0.5*(1-cos((2*M_PI*n)/(N-1)));
                        }
                        for(;k<traceSize;k++) {
                            n=k-traceSize-freqIndexHigh;
                            tempFilter[k][0]=0.5*(1-cos((2*M_PI*n)/(N-1)));
                        }
                        break;
                    case bandPass:
                        for(k=0;k<traceSize/2;k++) {
                            if(k>=freqIndexLow && k<=freqIndexHigh) {
                                n=k-freqIndexLow;
                                tempFilter[k][0]=0.5*(1-cos((2*M_PI*n)/(N-1)));
                            }
                        }
                        for(;k<traceSize;k++) {
                            if(k>=traceSize-freqIndexHigh && k<=traceSize-freqIndexLow) {
                                n=k-traceSize-freqIndexHigh;
                                tempFilter[k][0]=0.5*(1-cos((2*M_PI*n)/(N-1)));
                            }
                        }
                        break;
                    case highPass:
                        for(;k<traceSize/2+freqIndexLow;k++) {
                            n=k-freqIndexLow;
                            tempFilter[k][0]=0.5*(1-cos((2*M_PI*n)/(N-1)));
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
                        
                        break;
                    case bandPass:
                        
                        break;
                    case highPass:
                        
                        break;
                }
                break;
                //TODO:add here other windows
        }
        for(i=0;i<traceSize;i++) {
            filterFunction[i][0]+=tempFilter[i][0];
            filterFunction[i][1]+=tempFilter[i][1];
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
    fftwf_plan plan;
    Input input(inputTrace);
    input.readHeader();
    if(traceSize!=next_two_power(input.samplesPerTrace)) {
        cout<<"Different dimension of traces."<<endl;
        exit(0);
    }
    float delta;
    float** trace=new float*[input.numTraces];
    uint8_t** plain=new uint8_t*[input.numTraces];
    float* average=new float[traceSize];
    for(int w=0;w<input.numTraces;w++) {
        trace[w]=new float[input.samplesPerTrace];
        plain[w]=new uint8_t[input.plainLength];
    }
    for(int w=0;w<input.samplesPerTrace;w++)
        average[w]=0;
    int n=0;
    int count=0;
    float x;
    while(n<=input.numTraces) {
        input.readData(trace,plain,step);
        n+=step;
        //for every sample
        for(int i=0;i<input.samplesPerTrace;i++) {
            count=n-step;
            for(int j=0;j<step;j++) {
                count++;
                x=trace[j][i];
                delta=x-average[i];
                average[i]+=delta/count;
            }
        }
    }
    cout<<average[100]<<" "<<average[200]<<endl;
    switch(pad) {
        case zero:
            for(int n=input.samplesPerTrace;n<traceSize;n++)
                average[n]=0;
            break;
        case hold:
            for(int n=samplesPerTrace;n<traceSize;n++)
                average[n]=average[input.samplesPerTrace-1];
            break;
        case mean:
            float m;
            for(int i=0;i<step;i++) {
                m=computeMean(average,samplesPerTrace);
                for(int n=input.samplesPerTrace;n<traceSize;n++)
                    average[n]=m;
            }
            break;
    }
    plan=fftwf_plan_dft_r2c_1d(traceSize,average,filterFunction,FFTW_ESTIMATE);
    fftwf_execute(plan);
    //normalize the filter
    //TODO:find out how to normalize
    float modulus;
    for(int i=0;i<traceSize;i++) {
        modulus=sqrt(pow(filterFunction[i][0],2)+pow(filterFunction[i][1],2));
        filterFunction[i][0]/=modulus;
        filterFunction[i][1]/=modulus;
    }
    fftwf_destroy_plan(plan);
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
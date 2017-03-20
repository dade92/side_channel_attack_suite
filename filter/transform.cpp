#include"transform.hpp"

Transform::Transform(Config& config,Input& input,int ts,float** data) {
    windows=config.windows;
    dataMatrix=data;
    pad=config.pad;
    traceSize=ts;
    samplingFreq=config.samplingFreq;
    step=config.step;
    showFilter=config.plotFilter;
    samplesPerTrace=input.samplesPerTrace;
    fc=config.filterComb;
    alpha=config.alpha;
    demodularize=config.demodularize;
    //how many samples do I have to allocate?
    transformation=fftwf_alloc_complex(traceSize);
    filterFunction=fftwf_alloc_complex(traceSize);
    complex_input=fftwf_alloc_complex(traceSize);
    complex_output=fftwf_alloc_complex(traceSize);
    filterFunctionStop=fftwf_alloc_complex(traceSize);
    for(int i=0;i<traceSize;i++) {
        transformation[i][0]=transformation[i][1]=0;
        filterFunction[i][0]=filterFunction[i][1]=0;
        complex_output[i][0]=complex_output[i][1]=0;
        filterFunctionStop[i][0]=filterFunctionStop[i][1]=0;
    }
    if(demodularize) {
        buffer=fftwf_alloc_complex(traceSize);
        for(int i=0;i<traceSize;i++) {
            buffer[i][0]=buffer[i][1]=0;
        }
    }
    first=true;
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
    float a0=0.355768;
    float a1=0.487396;
    float a2=0.144232;
    float a3=0.012604;
    bool stop=false;
    //for each window, compute the filter specified by the user
    for(it=windows.begin();it!=windows.end();++it) {
        //init the windows index (k:N=f:F)
        freqIndexLow=it->lowFrequency*traceSize/samplingFreq;
        freqIndexHigh=it->highFrequency*traceSize/samplingFreq;
        N=abs(freqIndexHigh-freqIndexLow);
        if(N%2==0)
            N+=1;
        //TODO:manage even or odd window
        switch(it->windowFunction) {
            //here is a list of window functions
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
                        stop=true;
                        for(k=0;k<freqIndexHigh;k++)
                            filterFunctionStop[k][0]+=1;
                        for(k=traceSize-freqIndexHigh;k<traceSize;k++)
                            filterFunctionStop[k][0]+=1;
                        break;
                    case stopBand:
                        stop=true;
                        for(k=0;k<traceSize/2;k++) {
                            if(k>=freqIndexLow && k<=freqIndexHigh)
                                filterFunctionStop[k][0]+=1;
                        }
                        for(k=traceSize/2;k<traceSize;k++) {
                            if(k>=traceSize-freqIndexHigh && k<=traceSize-freqIndexLow)
                                filterFunctionStop[k][0]+=1;
                        }
                        break;
                }
                break;
            case hann:
                switch(it->type) {
                    case lowPass:
                        for(k=0;k<freqIndexHigh;k++) {
                            n=k+N;
                            filterFunction[k][0]+=generalized_hamming_window(0.5,0.5,n,2*N);
                        }
                        for(k=traceSize-freqIndexHigh;k<traceSize;k++) {
                            n=k-(traceSize-freqIndexHigh);
                            filterFunction[k][0]+=generalized_hamming_window(0.5,0.5,n,2*N);
                        }
                        break;
                    case bandPass:
                        for(k=0;k<traceSize/2;k++) {
                            if(k>=freqIndexLow && k<=freqIndexHigh) {
                                n=k-freqIndexLow;
                                filterFunction[k][0]+=generalized_hamming_window(0.5,0.5,n,N);
                            }
                        }
                        for(;k<traceSize;k++) {
                            if(k>=traceSize-freqIndexHigh && k<=traceSize-freqIndexLow) {
                                n=k-(traceSize-freqIndexHigh);
                                filterFunction[k][0]+=generalized_hamming_window(0.5,0.5,n,N);
                            }
                        }
                        break;
                    case highPass:
                        stop=true;
                        freqIndexHigh=freqIndexLow;
                        for(k=0;k<freqIndexLow;k++) {
                            n=k+N;
                            filterFunctionStop[k][0]+=generalized_hamming_window(0.5,0.5,n,2*N);
                        }
                        for(k=traceSize-freqIndexLow;k<traceSize;k++) {
                            n=k-(traceSize-freqIndexLow);
                            filterFunctionStop[k][0]+=generalized_hamming_window(0.5,0.5,n,2*N);
                        }
                        break;
                    case stopBand:
                        stop=true;
                        for(k=0;k<traceSize/2;k++) {
                            if(k>=freqIndexLow && k<=freqIndexHigh) {
                                n=k-freqIndexLow;
                                filterFunction[k][0]+=generalized_hamming_window(0.5,0.5,n,N);
                            }
                        }
                        for(;k<traceSize;k++) {
                            if(k>=traceSize-freqIndexHigh && k<=traceSize-freqIndexLow) {
                                n=k-(traceSize-freqIndexHigh);
                                filterFunction[k][0]+=generalized_hamming_window(0.5,0.5,n,N);
                            }
                        }
                        break;
                }
                break;
            case nuttall:
                switch(it->type) {
                    case lowPass:
                        for(k=0;k<freqIndexHigh;k++) {
                            n=k+N;
                            filterFunction[k][0]+=generalized_cosine_window(a0,a1,a2,a3,n,2*N);
                        }
                        for(k=traceSize-freqIndexHigh;k<traceSize;k++) {
                            n=k-(traceSize-freqIndexHigh);
                            filterFunction[k][0]+=generalized_cosine_window(a0,a1,a2,a3,n,2*N);
                        }
                        break;
                    case bandPass:
                        for(k=0;k<traceSize/2;k++) {
                            if(k>=freqIndexLow && k<=freqIndexHigh) {
                                n=k-freqIndexLow;
                                filterFunction[k][0]+=generalized_cosine_window(a0,a1,a2,a3,n,N);
                            }
                        }
                        for(;k<traceSize;k++) {
                            if(k>=traceSize-freqIndexHigh && k<=traceSize-freqIndexLow) {
                                n=k-(traceSize-freqIndexHigh);
                                filterFunction[k][0]+=generalized_cosine_window(a0,a1,a2,a3,n,N);
                            }
                        }
                        break;
                    case highPass:
                        stop=true;
                        freqIndexHigh=freqIndexLow;
                        for(k=0;k<freqIndexLow;k++) {
                            n=k+N;
                            filterFunctionStop[k][0]+=generalized_cosine_window(a0,a1,a2,a3,n,2*N);
                        }
                        for(k=traceSize-freqIndexLow;k<traceSize;k++) {
                            n=k-(traceSize-freqIndexLow);
                            filterFunctionStop[k][0]+=generalized_cosine_window(a0,a1,a2,a3,n,2*N);
                        }
                        break;
                    case stopBand:
                        stop=true;
                        for(k=0;k<traceSize/2;k++) {
                            if(k>=freqIndexLow && k<=freqIndexHigh) {
                                n=k-freqIndexLow;
                                    filterFunctionStop[k][0]+=generalized_cosine_window(a0,a1,a2,a3,n,N);
                            }
                        }
                        for(;k<traceSize;k++) {
                            if(k>=traceSize-freqIndexHigh && k<=traceSize-freqIndexLow) {
                                n=k-(traceSize-freqIndexHigh);
                                filterFunctionStop[k][0]+=generalized_cosine_window(a0,a1,a2,a3,n,N);
                            }
                        }
                        break;
                }
                break;
            case tukey:
                switch(it->type) {
                    case lowPass:
                        for(k=0;k<freqIndexHigh;k++) {
                            n=k+N;
                            filterFunction[k][0]+=tukey_window(alpha,n,2*N);
                        }
                        for(k=traceSize-freqIndexHigh;k<traceSize;k++) {
                            n=k-(traceSize-freqIndexHigh);
                            filterFunction[k][0]+=tukey_window(alpha,n,2*N);
                        }
                        break;
                    case bandPass:
                        for(k=0;k<traceSize/2;k++) {
                            if(k>=freqIndexLow && k<=freqIndexHigh) {
                                n=k-freqIndexLow;
                                filterFunction[k][0]+=tukey_window(alpha,n,N);
                            }
                        }
                        for(;k<traceSize;k++) {
                            if(k>=traceSize-freqIndexHigh && k<=traceSize-freqIndexLow) {
                                n=k-(traceSize-freqIndexHigh);
                                filterFunction[k][0]+=tukey_window(alpha,n,N);
                            }
                        }
                        break;
                    case highPass:
                        stop=true;
                        for(k=0;k<freqIndexLow;k++) {
                            n=k+N;
                            filterFunctionStop[k][0]+=tukey_window(alpha,n,2*N);
                        }
                        for(k=traceSize-freqIndexLow;k<traceSize;k++) {
                            n=k-(traceSize-freqIndexLow);
                            filterFunctionStop[k][0]+=tukey_window(alpha,n,2*N);
                        }
                        break;
                    case stopBand:
                        stop=true;
                        for(k=0;k<traceSize/2;k++) {
                            if(k>=freqIndexLow && k<=freqIndexHigh) {
                                n=k-freqIndexLow;
                                filterFunctionStop[k][0]+=tukey_window(alpha,n,N);
                            }
                        }
                        for(;k<traceSize;k++) {
                            if(k>=traceSize-freqIndexHigh && k<=traceSize-freqIndexLow) {
                                n=k-(traceSize-freqIndexHigh);
                                filterFunctionStop[k][0]+=tukey_window(alpha,n,N);
                            }
                        }
                        break;
                }   
            break;
            //add here other window types
        }
    }
    //generate the stop band filters
    if(stop) {
        for(k=0;k<traceSize;k++)
            filterFunction[k][0]+=1-filterFunctionStop[k][0];
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
        case normalize:
            float modulus;
            for(k=0;k<traceSize;k++) {
                modulus=sqrt(pow(filterFunction[k][0],2)+pow(filterFunction[k][1],2));
                filterFunction[k][0]/=modulus;
                filterFunction[k][1]/=modulus;
            }
            break;
    }
}

void Transform::computeFilter(string inputTrace) {
    FILE* fp=fopen(inputTrace.c_str(),"rb");
    if(fp==NULL) {
        cout<<"Can't open filter."<<endl;
        exit(0);
    }
    uint32_t numSamples;
    uint32_t numTraces;
    char sampleType;
    uint8_t pl;
    //reads the header
    fread(&numTraces,sizeof(int),1,fp);
    fread(&numSamples,sizeof(int),1,fp);
    fread(&sampleType,sizeof(char),1,fp);
    fread(&pl,sizeof(uint8_t),1,fp);
    cout<<"Num samples:"<<numSamples<<endl;
    if(numSamples!=traceSize) {
        cout<<"Different size of filter and traces."<<endl;
        exit(0);
    }
    //get  the filter from file
    if(sampleType=='c')
        fread(filterFunction,sizeof(fftwf_complex),numSamples,fp);
    else if(sampleType=='f') {
        float*filterFloat=new float[numSamples];
        fread(filterFloat,sizeof(float),numSamples,fp);
        for(int i=0;i<numSamples;i++) {
            filterFunction[i][0]=filterFloat[i];
            filterFunction[i][1]=0;
        }
    }
    else {
        cout<<"Unrecognized sample type."<<endl;
        exit(0);
    }
}

void Transform::filterTraces() {
    int i,n;
    if(showFilter)
        this->plotFilter(filterFunction);
    if(demodularize)
        cout<<"Starting filtering with demodulation..."<<endl;
    else 
        cout<<"Starting filtering..."<<endl;
    if(windows.size()==0)
        return;
    for(i=0;i<step;i++) {
        for(int w=0;w<traceSize;w++) {
            complex_input[w][0]=dataMatrix[i][w];
            complex_input[w][1]=0;
        }
        fftwf_plan plan=fftwf_plan_dft_1d(traceSize,complex_input,transformation,FFTW_FORWARD,FFTW_ESTIMATE);
        fftwf_execute(plan);
        for(n=0;n<traceSize;n++) {
            transformation[n][0]=transformation[n][0]*filterFunction[n][0] - 
                transformation[n][1]*filterFunction[n][1];
            transformation[n][1]=transformation[n][0]*filterFunction[n][1]+
                transformation[n][1]*filterFunction[n][0];
        }
        fftwf_destroy_plan(plan);
        fftwf_plan anti_plan;
        anti_plan=fftwf_plan_dft_1d(traceSize,transformation,complex_output,FFTW_BACKWARD,FFTW_ESTIMATE);
        fftwf_execute(anti_plan);
        for(int w=0;w<traceSize;w++)
            dataMatrix[i][w]=complex_output[w][0];
        fftwf_destroy_plan(anti_plan);
    }
    //normalize the transformation
    for(i=0;i<step;i++) {
        for(n=0;n<traceSize;n++)
                dataMatrix[i][n]/=traceSize;
    }
}

void Transform::demodulate() {
    vector<window>::iterator it;
    int freqIndexLow,freqIndexHigh,index,N,i,w;
    for(it=windows.begin();it!=windows.end();++it) {
        w=0;
        if(it->type==bandPass) {
            freqIndexLow=it->lowFrequency*traceSize/samplingFreq;
            freqIndexHigh=it->highFrequency*traceSize/samplingFreq;
            N=freqIndexHigh-freqIndexLow;
            if(N%2==0)
                N+=1;
            //take the bins and put at low frequencies (beginning of the frequency array)
            index=freqIndexLow+floor((double)N/2);
//             cout<<"index:"<<index<<endl;
            for(i=index;i<=traceSize/2;i++) {
                buffer[w][0]+=transformation[i][0];
                buffer[w][1]+=transformation[i][1];
                w++;
            }
            w=traceSize-1;
            //first traces go to the end
            for(i=index-1;i>=0;i--) {
                buffer[w][0]+=transformation[i][0];
                buffer[w][1]+=transformation[i][1];
                w--;
            }
        }
        else {
            cout<<"Some filter windows are not band pass."<<endl;
            exit(0);
        }
    }
}

void Transform::demodulate(float cutFrequency) {
    fftwf_complex* input=fftwf_alloc_complex(traceSize);
    fftwf_complex* demodulated_output=fftwf_alloc_complex(traceSize);
    fftwf_complex* filter=fftwf_alloc_complex(traceSize);
    fftwf_complex* output=fftwf_alloc_complex(traceSize);
    int i,n,k,N,freqIndexHigh;
    for(i=0;i<traceSize;i++) {
        input[i][1]=0;
        filter[i][0]=filter[i][1]=0;
        demodulated_output[i][0]=demodulated_output[i][1]=0;
    }
    N=freqIndexHigh=(cutFrequency)*traceSize/samplingFreq;
    //produces a lowPass filter
    for(k=0;k<freqIndexHigh;k++) {
        n=k+N;
        filter[k][0]+=generalized_hamming_window(0.5,0.5,n,2*N);
    }
    for(k=traceSize-freqIndexHigh;k<traceSize;k++) {
        n=k-(traceSize-freqIndexHigh);
        filter[k][0]+=generalized_hamming_window(0.5,0.5,n,2*N);
    }
//     this->plotFilter(filter);
    for(int s=0;s<step;s++) {  
        for(i=0;i<traceSize;i++)
            input[i][0]=pow(dataMatrix[s][i],2)*2;
        fftwf_plan plan=fftwf_plan_dft_1d(traceSize,input,output,FFTW_FORWARD,FFTW_ESTIMATE);
        fftwf_execute(plan);
        //apply the lowPass filter
        for(n=0;n<traceSize;n++) {
            output[n][0]=output[n][0]*filter[n][0] - 
                output[n][1]*filter[n][1];
            output[n][1]=output[n][0]*filter[n][1]+
                output[n][1]*filter[n][0];
        }
        fftwf_plan anti_plan=fftwf_plan_dft_1d(traceSize,output,demodulated_output,FFTW_BACKWARD,FFTW_ESTIMATE);
        fftwf_execute(anti_plan);
        for(i=0;i<traceSize;i++)
            dataMatrix[s][i]=sqrt(demodulated_output[i][0]/(traceSize));  
    }
}

void Transform::plotFilter(fftwf_complex* f) {
    std::ofstream spectrumStatistic,spectrumStatisticData;
    spectrumStatistic.open("spectrumwindow.gpl");
    spectrumStatisticData.open("spectrumwindow.dat");
    if(!spectrumStatistic.is_open() || !spectrumStatisticData.is_open()) {
        cout<<"Can't open output files."<<endl;
        exit(0);
    }
    spectrumStatistic << "set term png size 2048,850"<<endl;
    spectrumStatistic << "set output \""<< "spectrumWindow" <<".png\";" << endl;
    spectrumStatistic << "set autoscale;" << endl;
    spectrumStatistic << "set xrange ["<<-samplingFreq/2<<":"<<samplingFreq/2<<"];"<< endl;
    spectrumStatistic<<"plot ";
    spectrumStatistic << "\""<< "spectrumwindow.dat" << "\" ";
    spectrumStatistic << "u 1:2 ";
    spectrumStatistic << "t \"amplitude\" ";
    spectrumStatistic << "with lines linecolor \"black\";"<<endl<<endl;
    float mod;
    int i=traceSize;
        for(int n=traceSize/2;n<traceSize;n++) {
            mod=(sqrt(pow(f[n][0],2)+pow(f[n][1],2)));
            spectrumStatisticData<<-i*(samplingFreq/2)/traceSize<<" "<<20*log10(mod)<<endl;
            i-=2;
        }
        for(int n=0;n<traceSize/2;n++) {
            mod=(sqrt(pow(f[n][0],2)+pow(f[n][1],2)));
            spectrumStatisticData<<n*(samplingFreq)/traceSize<<" "<<20*log10(mod)<<endl;
        }
}
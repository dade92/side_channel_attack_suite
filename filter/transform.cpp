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
    demodularize=config.demodularize;
    //how many samples do I have to allocate?
    transformation=fftwf_alloc_complex(traceSize);
    filterFunction=fftwf_alloc_complex(traceSize);
    complex_input=fftwf_alloc_complex(traceSize);
    complex_output=fftwf_alloc_complex(traceSize);
    for(int i=0;i<traceSize;i++) {
        transformation[i][0]=transformation[i][1]=0;
        filterFunction[i][0]=filterFunction[i][1]=0;
        complex_output[i][0]=complex_output[i][1]=0;
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
    //for each window, compute the filter specified by the user
    for(it=windows.begin();it!=windows.end();++it) {
        //init the windows index (k:N=f:F)
        freqIndexLow=it->lowFrequency*traceSize/samplingFreq;
        freqIndexHigh=it->highFrequency*traceSize/samplingFreq;
        N=freqIndexHigh-freqIndexLow;
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
                        for(k=freqIndexLow;k<traceSize/2+(traceSize/2-freqIndexLow);k++)
                            filterFunction[k][0]+=1;
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
                        for(k=freqIndexLow;k<traceSize/2+(traceSize/2-freqIndexLow);k++) {
                            n=k-freqIndexLow;
                            filterFunction[k][0]+=generalized_hamming_window(0.5,0.5,n,2*N);
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
                        for(k=traceSize/2;k<traceSize;k++) {
                            if(k>=traceSize-freqIndexHigh && k<=traceSize-freqIndexLow) {
                                n=k-(traceSize-freqIndexHigh);
                                filterFunction[k][0]+=generalized_cosine_window(a0,a1,a2,a3,n,N);
                            }
                        }
                        break;
                    case highPass:
                    for(k=freqIndexLow;k<traceSize/2+(traceSize/2-freqIndexLow);k++) {
                            n=k-freqIndexLow;
                            filterFunction[k][0]+=generalized_cosine_window(a0,a1,a2,a3,n,2*N);
                        }                        
                        break;
                }
                break;
                //add here other window types
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
    int i,n;
    //uncomment these lines if you want to plot the filter function
    /*std::ofstream spectrumStatistic,spectrumStatisticData;
    spectrumStatistic.open("spectrumwindow.gpl");
    spectrumStatisticData.open("spectrumwindow.dat");
    if(!spectrumStatistic.is_open() || !spectrumStatisticData.is_open()) {
        cout<<"Can't open output files."<<endl;
        exit(0);
    }
    spectrumStatistic << "set term png size 1024,850"<<endl;
    spectrumStatistic << "set output \""<< "spectrumWindow" <<".png\";" << endl;
    spectrumStatistic << "set autoscale;" << endl;
    spectrumStatistic<<"plot ";
    spectrumStatistic << "\""<< "spectrumwindow.dat" << "\" ";
    spectrumStatistic << "u 1:2 ";
    spectrumStatistic << "t \"amplitude\" ";
    spectrumStatistic << "with lines linecolor \"black\";"<<endl<<endl;*/
    float mod;
    for(n=0;n<traceSize;n++) {
        mod=(sqrt(pow(filterFunction[n][0],2)+pow(filterFunction[n][1],2)));
        spectrumStatisticData<<n*samplingFreq/traceSize<<" "<<mod<<endl;
    }
    
    for(i=0;i<step;i++) {
        for(int w=0;w<traceSize;w++)
            complex_input[w][0]=dataMatrix[i][w];
        fftwf_plan plan=fftwf_plan_dft_1d(traceSize,complex_input,transformation,FFTW_FORWARD,FFTW_ESTIMATE);
        fftwf_execute(plan);
        /*if(first) {
            std::ofstream spectrumStatistic,spectrumStatisticData;
            spectrumStatistic.open("spectrum_before.gpl");
            spectrumStatisticData.open("spectrum_before.dat");
            if(!spectrumStatistic.is_open() || !spectrumStatisticData.is_open()) {
                cout<<"Can't open output files."<<endl;
                exit(0);
            }
            spectrumStatistic << "set term png size 1024,850"<<endl;
            spectrumStatistic << "set output \""<< "spectrum_before" <<".png\";" << endl;
            spectrumStatistic << "set autoscale;" << endl;
            spectrumStatistic<<"plot ";
            spectrumStatistic << "\""<< "spectrum_before.dat" << "\" ";
            spectrumStatistic << "u 1:2 ";
            spectrumStatistic << "t \"amplitude\" ";
            spectrumStatistic << "with lines linecolor \"black\";"<<endl<<endl;
            for(n=0;n<traceSize;n++) {
                mod=(sqrt(pow(transformation[n][0],2)+pow(transformation[n][1],2)));
                spectrumStatisticData<<n*samplingFreq/traceSize<<" "<<mod<<endl;
            }
        }*/
        for(n=0;n<traceSize;n++) {
            transformation[n][0]=transformation[n][0]*filterFunction[n][0] - 
                transformation[n][1]*filterFunction[n][1];
            transformation[n][1]=transformation[n][0]*filterFunction[n][1]+
                transformation[n][1]*filterFunction[n][0];
        }
        /*if(first) {
            std::ofstream spectrumStatistic,spectrumStatisticData;
            spectrumStatistic.open("spectrum_after.gpl");
            spectrumStatisticData.open("spectrum_after.dat");
            if(!spectrumStatistic.is_open() || !spectrumStatisticData.is_open()) {
                cout<<"Can't open output files."<<endl;
                exit(0);
            }
            spectrumStatistic << "set term png size 1024,850"<<endl;
            spectrumStatistic << "set output \""<< "spectrum_after" <<".png\";" << endl;
            spectrumStatistic << "set autoscale;" << endl;
            spectrumStatistic<<"plot ";
            spectrumStatistic << "\""<< "spectrum_after.dat" << "\" ";
            spectrumStatistic << "u 1:2 ";
            spectrumStatistic << "t \"amplitude\" ";
            spectrumStatistic << "with lines linecolor \"black\";"<<endl<<endl;
            for(n=0;n<traceSize;n++) {
                mod=(sqrt(pow(transformation[n][0],2)+pow(transformation[n][1],2)));
                spectrumStatisticData<<n*samplingFreq/traceSize<<" "<<mod<<endl;
            }
            first=false;
        }*/
        fftwf_destroy_plan(plan);
        fftwf_plan anti_plan;
        if(demodularize) {
            cout<<"Demodulating..."<<endl;
            anti_plan=fftwf_plan_dft_1d(traceSize,buffer,complex_output,FFTW_BACKWARD,FFTW_ESTIMATE);
            demodulate();
            fftwf_execute(anti_plan);
            /*for(int i=0;i<traceSize;i++) {
                complex_output[i][0]/=traceSize;
                complex_output[i][1]/=traceSize;
            }
            //after the conversion, i should normalize the time signal, before re-transforming again!
            fftwf_plan test=fftwf_plan_dft_1d(traceSize,complex_output,buffer,FFTW_BACKWARD,FFTW_ESTIMATE);
            fftwf_execute(test);*/
                /*std::ofstream spectrumStatistic,spectrumStatisticData;
    spectrumStatistic.open("demodulated.gpl");
    spectrumStatisticData.open("demodulated.dat");
    if(!spectrumStatistic.is_open() || !spectrumStatisticData.is_open()) {
                cout<<"Can't open output files."<<endl;
                exit(0);
    }
            spectrumStatistic << "set term png size 1024,850"<<endl;
            spectrumStatistic << "set output \""<< "demodulated" <<".png\";" << endl;
            spectrumStatistic << "set autoscale;" << endl;
            spectrumStatistic<<"plot ";
            spectrumStatistic << "\""<< "demodulated.dat" << "\" ";
            spectrumStatistic << "u 1:2 ";
            spectrumStatistic << "t \"amplitude\" ";
            spectrumStatistic << "with lines linecolor \"black\";"<<endl<<endl;*/
            /*float** testData=new float*[1];
            testData[0]=new float[traceSize];
            Input test("/home/davide/Documenti/matlab/sin_50_1000");
            test.readHeader();
            uint8_t** testPlain=new uint8_t*[1];
            testPlain[0]=new uint8_t[test.plainLength];
            test.readData(testData,testPlain,1);
            fftwf_plan test_plan;
            fftwf_complex* complex_test_input=fftwf_alloc_complex(traceSize);
            fftwf_complex* complex_test_output=fftwf_alloc_complex(traceSize);
            for(int i=0;i<traceSize;i++) {
                complex_test_input[i][0]=testData[0][i];
                complex_test_input[i][1]=0;
            }
            test_plan=fftwf_plan_dft_1d(traceSize,complex_test_input,complex_test_output,FFTW_FORWARD,FFTW_ESTIMATE);
            fftwf_execute(test_plan);*/
            for(int n=0;n<traceSize;n++) {
                mod=(sqrt(pow(buffer[n][0],2)+pow(buffer[n][1],2)));
//                 mod=(atan(buffer[n][1]/buffer[n][0]))-(atan(complex_test_output[n][1]/complex_test_output[n][0]));
                spectrumStatisticData<<n*samplingFreq/traceSize<<" "<<mod<<endl;
            }
        }
        else {
            anti_plan=fftwf_plan_dft_1d(traceSize,transformation,complex_output,FFTW_BACKWARD,FFTW_ESTIMATE);
            fftwf_execute(anti_plan);
        }
        for(int w=0;w<traceSize;w++)
            dataMatrix[i][w]=complex_output[w][0];
        fftwf_destroy_plan(anti_plan);
    }
    //normalize the transformation
    for(i=0;i<step;i++) {
        for(n=0;n<traceSize;n++)
                dataMatrix[i][n]/=traceSize;
    }
    /*{
    std::ofstream spectrumStatistic,spectrumStatisticData;
    spectrumStatistic.open("time_signal.gpl");
    spectrumStatisticData.open("time_signal.dat");
    if(!spectrumStatistic.is_open() || !spectrumStatisticData.is_open()) {
                cout<<"Can't open output files."<<endl;
                exit(0);
    }
            spectrumStatistic << "set term png size 2000,850"<<endl;
            spectrumStatistic << "set output \""<< "time_signal" <<".png\";" << endl;
            spectrumStatistic << "set autoscale;" << endl;
            spectrumStatistic << "set xrange [0:200]"<<endl;
            spectrumStatistic<<"plot ";
            spectrumStatistic << "\""<< "time_signal.dat" << "\" ";
            spectrumStatistic << "u 1:2 ";
            spectrumStatistic << "t \"amplitude\" ";
            spectrumStatistic << "with lines linecolor \"black\";"<<endl<<endl;
             for(int n=0;n<traceSize;n++) {
                spectrumStatisticData<<n<<" "<<complex_output[n][0]<<endl;
            }
    }*/  
    
}
//TODO:does not work well, why?
void Transform::demodulate() {
    vector<window>::iterator it;
    int freqIndexLow,freqIndexHigh,index,N,i;
    int w=0;
    for(it=windows.begin();it!=windows.end();++it) {
        if(it->type==bandPass) {
            freqIndexLow=it->lowFrequency*traceSize/samplingFreq;
            freqIndexHigh=it->highFrequency*traceSize/samplingFreq;
            N=freqIndexHigh-freqIndexLow;
            //take the bins and put at low frequencies (beginning of the frequency array)
            index=freqIndexLow+ceil((double)N/2);
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
            /*//shift the negative frequencies
            index=traceSize/2-index;
            //negative elements are shifted too, and arrived at the high frequencies
            for(i=traceSize-freqIndexLow;i>=traceSize/2;i--) {
                buffer[i-index][0]=transformation[i][0];
                buffer[i-index][1]=transformation[i][1];
                w--;
            }*/
        }
    }
}
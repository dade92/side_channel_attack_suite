#include"realigner.hpp"

Realigner::Realigner(Config& config,Input& input,float*ref) {
    step=config.step;
    samplesPerTrace=input.samplesPerTrace;
    maxTau=config.maxTau;
    function=config.function;
    refTrace=(fftwf_complex *) fftwf_malloc(sizeof(fftwf_complex) * (2 * samplesPerTrace - 1));
    refTraceTransform=(fftwf_complex *) fftwf_malloc(sizeof(fftwf_complex) * (2 * samplesPerTrace - 1));
    //store the very first trace
    for(int w=0;w<samplesPerTrace;w++) {
        refTrace[w][0]=ref[w];
        refTrace[w][1]=0;
    }
    for(int w=samplesPerTrace;w<2*samplesPerTrace-1;w++) {
        refTrace[w][0]=0;
        refTrace[w][1]=0;
    }
    fftwf_plan pa = fftwf_plan_dft_1d(2 * samplesPerTrace - 1,refTrace, refTraceTransform,FFTW_FORWARD,FFTW_ESTIMATE);
    fftwf_execute(pa);
    //TODO:remove these attributes if not used
    samplingFreq=config.samplingFreq;
    cipherTime=config.cipherTime;
}
/**
 * given a set of traces, it 
 * realignes them, computing the
 * tau in which their correlation
 * is maximized
 */
void Realigner::alignTraces(float** trace) {
    int tau,finalTau=0,max;
    float* correlation=new float[2*samplesPerTrace-1];
    complexTrace=(fftwf_complex *) fftwf_malloc(sizeof(fftwf_complex) * (2 * samplesPerTrace - 1));
    complexTraceTransform=(fftwf_complex *) fftwf_malloc(sizeof(fftwf_complex) * (2 * samplesPerTrace - 1));
    out_product=(fftwf_complex *) fftwf_malloc(sizeof(fftwf_complex) * (2 * samplesPerTrace - 1));
    correlationComplex=(fftwf_complex *) fftwf_malloc(sizeof(fftwf_complex) * (2 * samplesPerTrace - 1));
    //inserted here and not inside the for loop for performance reasons
    switch(function) {
        case crossCorr:
            //it does not use the function crossCorrelate in math.hpp because it is optimized for this case
            //(the very first trace is transformed only in the constructor, not in every iteration
            fftwf_plan pa = fftwf_plan_dft_1d(2 * samplesPerTrace - 1,
                                                  complexTrace,complexTraceTransform,FFTW_FORWARD,FFTW_ESTIMATE);
            fftwf_plan px =  fftwf_plan_dft_1d(2 * samplesPerTrace - 1,out_product
                ,correlationComplex,FFTW_BACKWARD,FFTW_ESTIMATE);
            for(int i=1;i<step;i++) {
                for(int w=0;w<samplesPerTrace;w++) {
                    complexTrace[w][0]=trace[i][w];
                    complexTrace[w][1]=0;
                }
                for(int w=samplesPerTrace;w<2*samplesPerTrace-1;w++) {
                    complexTrace[w][0]=0;
                    complexTrace[w][1]=0;
                }
                fftwf_execute(pa);
                for (int i = 0; i< 2 * samplesPerTrace - 1; i++) {
                    out_product[i][0] = refTraceTransform[i][0] * complexTraceTransform[i][0] 
                        - refTraceTransform[i][1]*(-complexTraceTransform[i][1]);
                    out_product[i][1] = refTraceTransform[i][0] * (-complexTraceTransform[i][1]) 
                        + refTraceTransform[i][1]*complexTraceTransform[i][0];
                }
                fftwf_execute(px);
                for (int i = 0; i<2 * samplesPerTrace - 1; i++)
                    correlation[i] = correlationComplex[(i + samplesPerTrace) 
                    % (2 * samplesPerTrace - 1)][0] / (2 * samplesPerTrace - 1);
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
                for(int i=0;i<2 * samplesPerTrace - 1;i++) {
                    outputDat<<i<<" ";
                    outputDat<<correlation[i]<<endl;
                }
                outputScript << "set term png size "<<1280<<","<<850<<endl;
                outputScript << "set output \""<< "correlation.png\";" << endl;
                outputScript << "set autoscale;" << endl;
                outputScript << "unset key;" << endl;
                outputScript << "set xlabel \"Tau\" font \",20\";" << endl;
                outputScript << "set ylabel \"correlation\" font \",20\";" << endl << endl;
                outputScript << "plot \""<<datName<<".dat\" with lines linecolor black"<<endl;
                max=correlation[samplesPerTrace-maxTau];
                //TODO:conclude modification
                for(tau=samplesPerTrace-maxTau;tau<samplesPerTrace+maxTau;tau++) {
                    if(correlation[tau]>max) {
                        max=correlation[tau];
                        finalTau=tau;
                    } else if(correlation[tau]==max) {
                        if(abs(tau-samplesPerTrace)<abs(finalTau-samplesPerTrace)) {
                            max=correlation[tau];
                            finalTau=tau;
                        }
                    }
                }
                finalTau-=(samplesPerTrace-1);
                cout<<"Obtained tau:"<<finalTau<<endl;
                shiftTrace(trace[i],trace[i],finalTau);
            }
            break;
        //here other function types
    }    
}
/**
 * given a trace, it shifts it
 * by tau positions
 */
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
#include <iostream>
#include <string>
#include <algorithm>
#include <stdlib.h>     /* atoi */
#include <fstream>
#include <math.h>
// #include <png.h>
#include <stdio.h>
#include <cstdint>
#include<fftw3.h>
#include<math.h>

#include "config.hpp"
#include "../common/input.hpp"
#define MAX 256

using namespace std;

typedef struct {
    double r;       // percent
    double g;       // percent
    double b;       // percent
}rgb;
typedef struct {
    double h;       // angle in degrees
    double s;       // percent
    double v;       // percent
} hsv;

void printUsage();
void savePersistence(uint32_t** persistence,int numSamples,Config&);
void showTraces(Config& config,Input& input,float** trace);
void inspectTraces(Config& config,Input& input);
void generateSpectrum(float**,Input& input,Config& config);
// inline void setRGB(png_byte *ptr, uint32_t val);

/**
 * Trace inspector tool: plot
 * a graph with the mean, a graph
 * with the dev standard and one
 * with some traces
 */
int main(int argc,char*argv[]) {
    if(argc<2) {
        printUsage();
        exit(0);
    }
    Config config(argv[1]);
    config.init();
    Input input(config.filename);
    input.readHeader();
    if(config.maxTrace==0) {
        if(input.numTraces%config.batch!=0) {
            cout<<"step size should be a divisor of the number of traces"<<endl;
            exit(0);
        } else if(input.samplesPerTrace<config.maxSample) {
            cout<<"Wrong max sample."<<endl;
            exit(0);
        }
    }
    else {
        if(config.maxTrace%config.batch!=0) {
            cout<<"step size should be a divisor of the number of traces"<<endl;
            exit(0);
        } else if(input.samplesPerTrace<config.maxSample) {
            cout<<"Wrong max sample."<<endl;
            exit(0);
        }
    }
    inspectTraces(config,input);
}
/*
void savePersistence(uint32_t** persistence,int numSamples,Config& config) {
    cout<<"Computing persistence"<<endl;
    png_structp png_ptr;
    png_infop info_ptr;
    png_byte color_type;
    png_byte* row = NULL;
    int width=numSamples;
    int height=MAX;
    string output=config.outputDir+"/persistence.png";
    FILE* fp=fopen(output.c_str(),"wb");
    if(!fp) {
        cout<<"Can't open persistence file"<<endl;
        exit(0);
    }
    //init structs
    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr || ! png_ptr) {
        cout<<"Cannot setup images."<<endl;
        exit(0);
    }
    //exception handling
    if (setjmp(png_jmpbuf(png_ptr)))
        exit(0);
    //link the struct with the opened file
    png_init_io(png_ptr, fp);
    
    if (setjmp(png_jmpbuf(png_ptr)))
        exit(0);
    //write the header
    png_set_IHDR(png_ptr, info_ptr, numSamples, MAX,
                     8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
                     PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    png_write_info(png_ptr, info_ptr);
    
    if (setjmp(png_jmpbuf(png_ptr)))
        exit(0);
    row = new png_byte[3 * width];
    //now it can write the image file
    for(int y=0;y<MAX;y++) {
        for(int x=0;x<numSamples;x++)
            setRGB(&(row[x*3]), persistence[y][x]);
        png_write_row(png_ptr,row);
    }    
    
    if (setjmp(png_jmpbuf(png_ptr)))
        exit(0);
    //cleanup libpng resources
    png_write_end(png_ptr, info_ptr);
    png_destroy_write_struct(&png_ptr,&info_ptr);
    free(row);
    fclose(fp);
}*/

/**
 * plots some traces to have a first sight
 * of them
 */
void showTraces(Config& config,Input& input,float** trace) {
    std::ofstream outputStatistics,outputStatisticsData;
    outputStatistics.open(config.outputDir+"/traces.gpl");
    outputStatisticsData.open(config.outputDir+"/traces.dat");
    if(!outputStatistics.is_open() || !outputStatisticsData.is_open()) {
        cout<<"Can't open output files."<<endl;
        exit(0);
    }
    int maxSample=(config.maxSample!=0 ? config.maxSample : input.samplesPerTrace);
    if(config.latexOutput) {
        outputStatistics << "set term epslatex size "<<config.figureWidth<<" ,"<<config.figureHeight<<";"<<endl;
        outputStatistics << "set output \""<< "traces" <<".tex\";" << endl;
    }
    else {
        outputStatistics << "set term png size "<<config.figureWidth<<" ,"<<config.figureHeight<<";"<<endl;
        outputStatistics << "set output \""<< "traces" <<".png\";" << endl;
    }
    outputStatistics << "set autoscale;" << endl;
    outputStatistics << "set xtics auto font \",20\";" << endl;
    switch(config.unit) {
        case samples:
            outputStatistics << "set xrange ["<<config.startSample<<":"<<maxSample<<"];"<<endl;
            outputStatistics << "set xlabel \"Time\" font \",20\";" << endl;
            break;
        case seconds:
            outputStatistics << "set xrange ["
            <<(config.startSample/config.samplingFreq)*1000000<<":"
            <<(maxSample/config.samplingFreq)*1000000<<"];"<<endl;
            outputStatistics << "set xlabel \"Time[us]\" font \",20\";" << endl;
            break;
        
    }
    outputStatistics << "set ylabel \"\""<<endl;
    outputStatistics << "set xlabel \"Time\""<<endl;
    if(config.grid) {
        if(config.xtics==0) {
            switch(config.unit) {
                case samples:
                    outputStatistics << "set xtics "<<config.samplingFreq/config.clockFreq<< " format \"\";" << endl;
                    break;
                case seconds:
                    outputStatistics << "set xtics "<<1/config.clockFreq<< " format \"\";" << endl;
                    break;
            }
            
        }
        else
            outputStatistics << "set xtics "<<config.xtics<< " font \",20\";" << endl;
        outputStatistics<<"set grid xtics "<<" lt rgb \"grey\" lw 1;"<<endl;
        outputStatistics<<"set grid ytics "<<" lt rgb \"grey\" lw 1;"<<endl;
    }
    else {
        if(config.xtics==0)
            outputStatistics << "set xtics auto font \",20\";" << endl;
        else
            outputStatistics << "set xtics "<<config.xtics<<" font \",20\";" << endl;
    }
    outputStatistics << "plot ";
    for(int i=0;i<config.tracesToPrint;i++) {
        outputStatistics << "\"traces.dat\" u 1:"<<i+2<<" with lines";
        if(i==config.tracesToPrint-1)
            outputStatistics<<";";
        else
            outputStatistics<<",";
    }
    //write the .dat file
    for(int i=0;i<maxSample-config.startSample;i++) {
        switch(config.unit) {
            case samples:
                outputStatisticsData<<i+config.startSample<<" ";
                break;
            case seconds:
                outputStatisticsData<<(i+config.startSample)*1000000/config.samplingFreq<<" ";
                break;
        }
        for(int n=0;n<config.tracesToPrint;n++) {
            outputStatisticsData<<trace[n][i+config.startSample]<<" ";
        }
        outputStatisticsData<<endl;
    }
    
}
/**
 * plots mean dev standard and spectrum
 * of the traces
 */
void inspectTraces(Config& config,Input& input) {
    int step=config.batch;
    float** trace=new float*[step];
    uint8_t** plain=new uint8_t*[step];
    bool grid=config.grid;
    int numTraces;
    if(config.maxTrace==0)
        numTraces=input.numTraces;
    else
        numTraces=config.maxTrace;
    int maxSample=(config.maxSample!=0 ? config.maxSample : input.samplesPerTrace);
    int numSamples=maxSample-config.startSample;
    float* mean=new float[numSamples];
    float* var=new float[numSamples];
    float** temp=new float*[1];
    temp[0]=new float[input.samplesPerTrace];
//     uint16_t index;
//     int16_t integer_sample;
//     int matrix_index;
    float delta;
    int count=0;
    for(int i=0;i<step;i++) {
        trace[i]=new float[input.samplesPerTrace];
        plain[i]=new uint8_t[input.plainLength];
    }
    //persistence matrix
    uint32_t** persistence=new uint32_t*[MAX];
    for(int x=0;x<MAX;x++)
        persistence[x]=new uint32_t[numSamples];
    for(int w=0;w<MAX;w++) {
        for(int x=0;x<numSamples;x++) {
            persistence[w][x]=0;
        }
    }
    for(int n=0;n<numSamples;n++) {
        mean[n]=0;
	var[n]=0;
    }
    float x;
    switch(config.unit) {
        case samples:
            cout<<"computing mean from sample "<<config.startSample<<" to sample "<<maxSample<<endl;
            break;
        case seconds:
            cout<<"computing mean from second "
            <<config.startSample/config.samplingFreq
            <<" to second "<<maxSample/config.samplingFreq<<endl;
            break;
    }
    //I know the total number of samples for each instant in time: it is numTraces
    int n=0;
    while(n<numTraces) {
        input.readData(trace,plain,step);
        /*for(int y=0;y<step;y++) {
            for(int z=0;z<input.samplesPerTrace;z++)
                trace[y][z]=trace2[y][z];
        }*/
        if(!n)
            showTraces(config,input,trace);
        n+=step;
        /*cout<<"Plaintext:"<<endl;
        for(int x=0;x<16;x++)
                printf("0x%x ",plain[0][x]);
        cout<<endl;
        for(int x=0;x<16;x++)
                printf("0x%x ",plain[1][x]);
        cout<<endl;
        for(int x=0;x<16;x++)
                printf("0x%x ",plain[2][x]);
        cout<<endl;*/
        //for every sample
        for(int i=0;i<numSamples;i++) {
            count=n-step;
            for(int j=0;j<step;j++) {
                count++;
                x=trace[j][i+config.startSample];
                delta=x-mean[i];
                mean[i]+=delta/count;
                var[i]+=delta*(x-mean[i]);
//                 integer_sample=(int16_t) x;
//                 matrix_index = integer_sample/256 + 128;
//                 persistence[matrix_index][i]++;
            }
        }
    }
    //savePersistence(persistence,numSamples,config);
    //compute the dev standard
    for(int i=0;i<numSamples;i++) {
	var[i]/=(numTraces-1);
	var[i]=sqrt(var[i]);
    }
    cout<<"Computing the spectrum.."<<endl;
    generateSpectrum(temp,input,config);
    cout<<"plotting the spectrum from frequency "<<config.startBin<<" to frequency "<<config.endBin<<endl;
    std::ofstream outputStatistics,outputStatisticsData;
    outputStatistics.open(config.outputDir+"/meanAndVariance.gpl");
    outputStatisticsData.open(config.outputDir+"/meanAndVariance.dat");
    
    if(!outputStatistics.is_open() || !outputStatisticsData.is_open()) {
        cout<<"Can't open output files."<<endl;
        exit(0);
    }
    //plot the mean .png
    if(config.latexOutput) {
        outputStatistics << "set term epslatex size "<<config.figureWidth<<" ,"<<config.figureHeight<<";"<<endl;
        outputStatistics << "set output \""<< "mean" <<".tex\";" << endl;
    }
    else {
        outputStatistics << "set term png size "<<config.figureWidth<<" ,"<<config.figureHeight<<";"<<endl;
        outputStatistics << "set output \""<< "mean" <<".png\";" << endl;
    }
    outputStatistics << "set autoscale;" << endl;
    if(grid) {
        if(config.xtics==0) {
            switch(config.unit) {
                case samples:
                    outputStatistics << "set xtics "<<config.samplingFreq/config.clockFreq<< " format \"\";" << endl;
                    break;
                case seconds:
                    outputStatistics << "set xtics "<<1/config.clockFreq<< " format \"\";" << endl;
                    break;
            }
            
        }
        else
            outputStatistics << "set xtics "<<config.xtics<< " font \",20\";" << endl;
        outputStatistics<<"set grid xtics "<<" lt rgb \"grey\" lw 1;"<<endl;
        outputStatistics<<"set grid ytics "<<" lt rgb \"grey\" lw 1;"<<endl;
    }
    else {
        if(config.xtics==0)
            outputStatistics << "set xtics auto font \",20\";" << endl;
        else
            outputStatistics << "set xtics "<<config.xtics<<" font \",20\";" << endl;
    }
    outputStatistics << "set ytic auto font \",20\";" << endl;
    switch(config.unit) {
        case samples:
            outputStatistics << "set xrange ["<<config.startSample<<":"<<maxSample<<"];"<<endl;
            outputStatistics << "set xlabel \"Time\" font \",20\";" << endl;
            break;
        case seconds:
            outputStatistics << "set xrange ["
            <<(config.startSample/config.samplingFreq)*1000000<<":"
            <<(maxSample/config.samplingFreq)*1000000<<"];"<<endl;
            outputStatistics << "set xlabel \"Time[us]\" font \",20\";" << endl;
            break;
        
    }
    outputStatistics << "unset key;" << endl;
    outputStatistics << "set ylabel \"Mean\" font \",20\";" << endl;
    outputStatistics<<"plot ";
    outputStatistics << "\""<< "meanAndVariance.dat" << "\" ";
    outputStatistics << "u 1:2 ";
    outputStatistics << "t \"mean\" ";
    outputStatistics << "with lines linecolor \"black\";"<<endl<<endl;
    
    
    //plot the standard deviation .png
    if(config.latexOutput) {
        outputStatistics << "set term epslatex size "<<config.figureWidth<<" ,"<<config.figureHeight<<";"<<endl;
        outputStatistics << "set output \""<< "standardDev" <<".tex\";" << endl;
    }
    else {
        outputStatistics << "set term png size "<<config.figureWidth<<" ,"<<config.figureHeight<<";"<<endl;
        outputStatistics << "set output \""<< "standardDev" <<".png\";" << endl;
    }
    outputStatistics << "set autoscale;" << endl;
    if(grid) {
        if(config.xtics==0) {
            switch(config.unit) {
                case samples:
                    outputStatistics << "set xtics "<<config.samplingFreq/config.clockFreq<< " format \"\";" << endl;
                    break;
                case seconds:
                    outputStatistics << "set xtics "<<1/config.clockFreq<< " format \"\";" << endl;
                    break;
            }
        }
        else
            outputStatistics << "set xtics "<<config.xtics<< " font \",20\";" << endl;
        outputStatistics<<"set grid xtics "<<" lt rgb \"grey\" lw 1;"<<endl;
        outputStatistics<<"set grid ytics "<<" lt rgb \"grey\" lw 1;"<<endl;
    }
    else {
        if(config.xtics==0)
            outputStatistics << "set xtics auto font \",20\";" << endl;
        else
            outputStatistics << "set xtics "<<config.xtics<<" font \",20\";" << endl;
    }
    outputStatistics << "set ytic auto font \",20\";" << endl;
    switch(config.unit) {
        case samples:
            outputStatistics << "set xrange ["<<config.startSample<<":"<<maxSample<<"];"<<endl;
            outputStatistics << "set xlabel \"Time\" font \",20\";" << endl;
            break;
        case seconds:
            outputStatistics << "set xrange ["
            <<(config.startSample/config.samplingFreq)*1000000<<":"
            <<(maxSample/config.samplingFreq)*1000000<<"];"<<endl;
            outputStatistics << "set xlabel \"Time[us]\" font \",20\";" << endl;
            break;
        
    }
    if(numTraces>1) {
        outputStatistics << "unset key;" << endl;
        outputStatistics << "set ylabel \"standard deviation\" font \",20\";" << endl;
        outputStatistics<<"plot ";
        outputStatistics << "\""<< "meanAndVariance.dat" << "\" ";
        outputStatistics << "u 1:3 ";
        outputStatistics << "t \"dev standard\" ";
        outputStatistics << "with lines linecolor \"black\";"<<endl<<endl;
    }
        //write data file
        for(int i=0;i<numSamples;i++) {
            switch(config.unit) {
                case samples:
                    outputStatisticsData<<i+config.startSample<<" ";
                    break;
                case seconds:
                    outputStatisticsData<<((i+config.startSample)/config.samplingFreq)*1000000<<" ";
                    break;
            }
            outputStatisticsData<<mean[i]<<" "<<var[i]<<endl;
    }
    cout<<"traces inspected. You can find gnuplot script in \"inspectTrace.gpl\" and in \"meanAndVariance.gpl\" "
    		"data in \"inspectTrace.dat\" and \"meanAndVariance.dat\" "<<endl;
}

void generateSpectrum(float** trace,Input& input,Config& config) {
    int start=config.startSample;
    int end=(config.maxSample!=0 ? config.maxSample : input.samplesPerTrace);
    std::ofstream spectrumStatistic,spectrumStatisticData;
    spectrumStatistic.open(config.outputDir+"/spectrum.gpl");
    spectrumStatisticData.open(config.outputDir+"/spectrum.dat");
    if(!spectrumStatistic.is_open() || !spectrumStatisticData.is_open()) {
        cout<<"Can't open output files."<<endl;
        exit(0);
    }
    if(config.latexOutput) {
        spectrumStatistic <<"set terminal epslatex size "<<config.figureWidth<<" ,"<<config.figureHeight<<";"<<endl;
        spectrumStatistic << "set output \""<< "spectrum" <<".tex\";" << endl;
    }
    else {
        spectrumStatistic << "set term png size "<<config.figureWidth<<" ,"<<config.figureHeight<<";"<<endl;
        spectrumStatistic << "set output \""<< "spectrum" <<".png\";" << endl;
    }
    spectrumStatistic << "set autoscale;" << endl;
    if(config.grid) {
        if(config.xtics==0) {
            switch(config.unit) {
                case samples:
                    spectrumStatistic << "set xtics "<<config.samplingFreq/config.clockFreq<< " format \"\";" << endl;
                    break;
                case seconds:
                    spectrumStatistic << "set xtics "<<1/config.clockFreq<< " format \"\";" << endl;
                    break;
            }
            
        }
        else
            spectrumStatistic << "set xtics "<<config.xtics<< " font \",20\";" << endl;
        spectrumStatistic<<"set grid xtics "<<" lt rgb \"grey\" lw 1;"<<endl;
        spectrumStatistic<<"set grid ytics "<<" lt rgb \"grey\" lw 1;"<<endl;
    }
    else {
        if(config.xtics==0)
            spectrumStatistic << "set xtics auto font \",20\";" << endl;
        else
            spectrumStatistic << "set xtics "<<config.xtics<<" font \",20\";" << endl;
    }
    spectrumStatistic << "set ytic auto font \",20\";" << endl;
    spectrumStatistic << "unset key;" << endl;
    spectrumStatistic << "set xlabel \"Frequency [Hz]\" font \",20\";" << endl;
    spectrumStatistic << "set ylabel \"Amplitude [dB]\" font \",20\";" << endl;
    spectrumStatistic << "set xrange ["<<config.startBin<<":"<<config.endBin<<"];"<<endl;
    spectrumStatistic<<"plot ";
    spectrumStatistic << "\""<< "spectrum.dat" << "\" ";
    spectrumStatistic << "u 1:2 ";
    spectrumStatistic << "t \"amplitude\" ";
    spectrumStatistic << "with lines linecolor \"black\";"<<endl<<endl;
    Input input2(config.filename);
    input2.readHeader();
    uint8_t** plain=new uint8_t*[1];
    for(int i=0;i<1;i++) {
        plain[i]=new uint8_t[input2.plainLength];
    }
    input2.readData(trace,plain,1);
    fftwf_plan plan;
    int traceSize=input2.samplesPerTrace;
    fftwf_complex* transformation=fftwf_alloc_complex(traceSize);
    fftwf_complex* trace_complex=fftwf_alloc_complex(traceSize);
    for(int i=0;i<traceSize;i++) {
        trace_complex[i][0]=trace[0][i];
        trace_complex[i][1]=0;
    }
    plan=fftwf_plan_dft_1d(traceSize,trace_complex,transformation,FFTW_FORWARD,FFTW_ESTIMATE);
    fftwf_execute(plan);
    float mod,maxFreq=config.samplingFreq;
    int i=(traceSize);
    int increment;
    if(config.latexOutput)
        increment=20;
    else
        increment=1;
    //i:traceSize=f:maxFreq
    if(config.logScale) {
        for(int n=traceSize/2;n<traceSize;n+=increment) {
            mod=(sqrt(pow(transformation[n][0],2)+pow(transformation[n][1],2)));
            spectrumStatisticData<<-i*(maxFreq/2)/traceSize<<" "<<20*log10(mod)<<endl;
            i-=increment*2;
        }
        for(int n=0;n<=traceSize/2;n+=increment) {
            mod=(sqrt(pow(transformation[n][0],2)+pow(transformation[n][1],2)));
            spectrumStatisticData<<n*(maxFreq)/traceSize<<" "<<20*log10(mod)<<endl;
        }
    } else {
        for(int n=traceSize/2;n<traceSize;n+=increment) {
            mod=(sqrt(pow(transformation[n][0],2)+pow(transformation[n][1],2)));
            spectrumStatisticData<<-i*(maxFreq/2)/traceSize<<" "<<(mod)<<endl;
            i-=increment*2;
        }
        for(int n=0;n<=traceSize/2;n+=increment) {
            mod=(sqrt(pow(transformation[n][0],2)+pow(transformation[n][1],2)));
            spectrumStatisticData<<n*(maxFreq)/traceSize<<" "<<(mod)<<endl;
        }
    }
    fftwf_destroy_plan(plan);
}

void printUsage() {
    cout<<"Usage:\n./traceInspector.out configFile"<<endl;
}
/*
inline void setRGB(png_byte *ptr, uint32_t val) {
        ptr[0]=val/pow(256,2);
        ptr[1]=val/256 % (uint32_t)pow(256,2);
        ptr[2]=val%256;
}*/

#include <iostream>
#include <string>
#include <algorithm>
#include <stdlib.h>     /* atoi */
#include <fstream>
#include <math.h>
#include <png.h>
#include <stdio.h>
#include <cstdint>

#include "config.hpp"
#include "../common/input.hpp"
//TODO:substitute MAX with meaningful name
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

void printUsage() {
    cout<<"./traceInspector.out configFile"<<endl;
}

inline void setRGB(png_byte *ptr, uint32_t val) {
        ptr[0]=val/pow(256,2);
        ptr[1]=val/256 % (uint32_t)pow(256,2);
        ptr[2]=val%256;
}

void savePersistence(uint32_t** persistence,int numSamples) {
    cout<<"Computing persistence"<<endl;
    png_structp png_ptr;
    png_infop info_ptr;
    png_byte color_type;
    png_byte* row = NULL;
    int width=numSamples;
    int height=MAX;
    FILE* fp=fopen("persistence.png","wb");
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
}
 
void inspectTraces(Config& config,Input& input) {
    int step=config.batch;
    float** trace=new float*[step];
    uint8_t** plain=new uint8_t*[step];
    bool grid=config.grid;
    int numTraces=input.numTraces;
    int maxSample=(config.maxSample!=0 ? config.maxSample : input.samplesPerTrace);
    int numSamples=maxSample-config.startSample;
    float* mean=new float[numSamples];
    float* var=new float[numSamples];
    uint16_t index;
    int16_t integer_sample;
    int matrix_index;
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
            <<" to sample "<<maxSample/config.samplingFreq<<endl;
            break;
    }
    //I know the total number of samples for each instant in time: it is numTraces
    int n=0;
    while(n<=numTraces) {
        input.readData(trace,plain,step);
        n+=step;
        //for every sample
        for(int i=0;i<numSamples;i++) {
            count=n-step;
            for(int j=0;j<step;j++) {
                count++;
                x=trace[j][i+config.startSample];
                delta=x-mean[i];
                mean[i]+=delta/count;
                var[i]+=delta*(x-mean[i]);
                integer_sample=(int16_t) x;
                matrix_index = integer_sample/256 + 128;
                persistence[matrix_index][i]++;
            }
        }
    }
    savePersistence(persistence,numSamples);
    //compute the dev standard
    for(int i=0;i<numSamples;i++) {
	var[i]/=(numTraces-1);
	var[i]=sqrt(var[i]);
    }
    std::ofstream outputStatistics,outputStatisticsData;
    outputStatistics.open("meanAndVariance.gpl");
    outputStatisticsData.open("meanAndVariance.dat");
    
    if(!outputStatistics.is_open() || !outputStatisticsData.is_open()) {
        cout<<"Can't open output files."<<endl;
        exit(0);
    }
    //plot the mean .png
    outputStatistics << "set term png size "<<config.figureWidth<<" ,"<<config.figureHeight<<";"<<endl;
    outputStatistics << "set output \""<< "mean" <<".png\";" << endl;
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
            <<config.startSample/config.samplingFreq<<":"
            <<maxSample/config.samplingFreq<<"];"<<endl;
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
    outputStatistics << "set term png size "<<config.figureWidth<<" ,"<<config.figureHeight<<";"<<endl;
    outputStatistics << "set output \""<< "standardDev" <<".png\";" << endl;
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
            <<config.startSample/config.samplingFreq<<":"
            <<maxSample/config.samplingFreq<<"];"<<endl;
            outputStatistics << "set xlabel \"Time[us]\" font \",20\";" << endl;
            break;
        
    }
    outputStatistics << "unset key;" << endl;
    outputStatistics << "set ylabel \"standard deviation\" font \",20\";" << endl;
    outputStatistics<<"plot ";
    outputStatistics << "\""<< "meanAndVariance.dat" << "\" ";
    outputStatistics << "u 1:3 ";
    outputStatistics << "t \"dev standard\" ";
    outputStatistics << "with lines linecolor \"black\";"<<endl<<endl;
    //write data file
    for(int i=0;i<numSamples;i++) {
        switch(config.unit) {
            case samples:
                outputStatisticsData<<i+config.startSample<<" ";
                break;
            case seconds:
                outputStatisticsData<<(i+config.startSample)/config.samplingFreq<<" ";
                break;
        }
	outputStatisticsData<<mean[i]<<" "<<var[i]<<endl;
    }
    cout<<"traces inspected. You can find gnuplot script in \"inspectTrace.gpl\" and in \"meanAndVariance.gpl\" "
    		"data in \"inspectTrace.dat\" and \"meanAndVariance.dat\" "<<endl;
}

int main(int argc,char*argv[]) {
    if(argc<2) {
        printUsage();
        exit(0);
    }
    Config config(argv[1]);
    config.init();
    Input input(config.filename);
    input.readHeader();
    if(input.numTraces%config.batch!=0) {
        cout<<"batch size should be a divisor of the number of traces"<<endl;
        exit(0);
    } else if(input.samplesPerTrace<config.maxSample) {
        cout<<"Wrong max sample."<<endl;
        exit(0);
    }
    inspectTraces(config,input);
}

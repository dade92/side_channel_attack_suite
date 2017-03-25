#ifndef OUTPUT_H_
#define OUTPUT_H_
#include<stdio.h>
#include<cstdint>
#include<iostream>
#include<string>
#include"input.hpp"
#define SAMPLE_FORMAT_FLOAT 'f'
#define SAMPLE_FORMAT_DOUBLE 'd'
#define SAMPLE_FORMAT_CHAR 'c'
/**
 * Output class of the filter tool, save a .dat file
 * containing the filtered traces
 */

class Output {
public:
    Output(string outputFile,int step,
           Input& input,float** data,uint8_t**plains);
    //in some cases I need to store objects without having a compatible .dat input file
    Output(string outputFile,int step,char format,
           int numTraces,int samplesPerTrace,int plaintLength,float**data,uint8_t**plains);
    Output(string outputFile,int step,
           Input& input,double** data,uint8_t**plains);
    Output(string outputFile,int step,char format,
           int numTraces,int samplesPerTrace,int plaintLength,double**data,uint8_t**plains);
    ~Output();
    void writeHeader();
    void writeTraces();
    void rewindFile();
    void setNumOfTraces(uint32_t numTraces);
    void setNumOfSamples(uint32_t numSamples);
    void setPlainLength(uint8_t );
    void setDataBuffer(float**buffer);
    void setPlainBuffer(uint8_t**);
    void rewriteHeader();
private:
    const char* filename;
    uint32_t samplesPerTrace,numTraces;
    uint8_t format;
    uint8_t plainLength;
    float** dataMatrix;
    double** dataMatrix2;
    uint8_t** plaintext;
    int step;
    FILE* fp;
};
#endif
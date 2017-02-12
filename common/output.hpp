#ifndef OUTPUT_H_
#define OUTPUT_H_
#include<stdio.h>
#include<cstdint>
#include<iostream>
#include<string>
#include"input.hpp"

/**
 * Output class of the filter tool, save a .dat file
 * containing the filtered traces
 */

class Output {
public:
    Output(string outputFile,int step,
           Input& input,float** data,uint8_t**plains);
    //in some cases I need to store objects without having a compatible .dat input file
    Output(string outputFile,int step,
           int numTraces,int samplesPerTrace,int plaintLength,float**data,uint8_t**plains);
    ~Output();
    void writeHeader();
    void writeTraces();
    void rewindFile();
    void setNumOfTraces(int numTraces);
private:
    const char* filename;
    uint32_t samplesPerTrace,numTraces;
    uint8_t format;
    uint8_t plainLength;
    float** dataMatrix;
    uint8_t** plaintext;
    int step;
    FILE* fp;
};
#endif
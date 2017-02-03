#ifndef OUTPUT_H_
#define OUTPUT_H_
#include<stdio.h>
#include<cstdint>
#include<iostream>
#include<string>
#include"config.hpp"
#include"../common/input.hpp"

/**
 * Output class of the filter tool, save a .dat file
 * containing the filtered traces
 */

class Output {
public:
    Output(Config& config,Input& input,float** data,uint8_t** plains);
    void writeHeader();
    void writeTraces();
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
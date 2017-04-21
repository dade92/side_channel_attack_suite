#ifndef TRACE_SPLITTER_H_
#define TRACE_SPLITTER_H_
#include<iostream>
#include<cstdint>
#include<string>
#include<string.h>
#include<stdio.h>
#include"../common/output.hpp"
#include"../common/input.hpp"
#include"../aes/aes.hpp"
#include"config.hpp"

using namespace std;

/**
 * Class of the trace alignment tool
 * that divide the long trace in different
 * AES traces based on correlation
 * computed before.
 * NOTE:needs one single long trace
 * in input, used only in single mode
 */
class TraceSplitter {
public:
    TraceSplitter(Config&,Input&,uint8_t**plain);
    int splitTrace(float*,float**,Output&,int& numSamples,bool& first);
private:
    float cipherTime,samplingFreq;
    int findMaxIndex(float*,int,int);
    int samplesPerTrace,plainLength,numSamples;
    string outputFilename;
    uint8_t*startPlain;
    uint8_t*key;
};
#endif
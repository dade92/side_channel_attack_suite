#ifndef TRANSFORM_H_
#define TRANSFORM_H_
#include<iostream>
#include<vector>
#include<fftw3.h>
#include<math.h>
#include<string>
#include<cstdint>
#include"config.hpp"
#include"../common/input.hpp"
#include"../common/math.hpp"
using namespace std;

/**
 * Class in the filter tool that compute 
 * the DFT of the traces, 
 * compute the correct filter by the parameters 
 * passed by the user in the config file, pads
 * the traces
 */

class Transform {
public:
    Transform(Config& config,Input& input,int traceSize,float** dataMatrix);
    void computeFilter();
    void computeFilter(string inputTrace);
    void padTraces();
    void filterTraces();
    
private:
    fftwf_complex* filterFunction;
    fftwf_complex* tempFilter;
    float** dataMatrix;
    float samplingFreq;
    padding pad;
    filterCombination fc;
    vector<window> windows;
    fftwf_complex* transformation;
    fftwf_complex* filter;
    int traceSize,samplesPerTrace,step;
};
#endif
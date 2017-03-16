#ifndef REALIGNER_H_
#define REALIGNER_H_
#include<iostream>
#include<string>
#include<fstream>
#include<fftw3.h>
#include<math.h>
#include"config.hpp"
#include"../common/input.hpp"
#include"../common/math.hpp"
#include"../common/output.hpp"

using namespace std;
/**
 * Config class for the filter tool,
 * parses the config file and put 
 * the parameters in public attributes
 */

class Realigner {
public:
    Realigner(Config& config,Input& input,float*ref);
    void alignTraces(float** trace);
    void divideTrace();
private:
    float correlate(float* ,float*);
    void shiftTrace(float* trace,float* shiftedTrace,int tau);
    int samplesPerTrace,step;
    bool printCorr;
    alignmentFunction function;
    fftwf_complex* refTrace;
    fftwf_complex* refTraceTransform;
    fftwf_complex* complexTrace;
    fftwf_complex* complexTraceTransform;
    fftwf_complex* correlationComplex;
    fftwf_complex* out_product;
    float samplingFreq,cipherTime;
    int maxTau;
    string plaintext;
};
#endif
#ifndef REALIGNER_H_
#define REALIGNER_H_
#include<iostream>
#include<string>
#include<fstream>
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
    void autoCorrelate(float* correlation);
    void divideTrace();
private:
    float correlate(float* ,float*);
    void shiftTrace(float* trace,float* shiftedTrace,int tau);
    int startSample,endSample,samplesPerTrace,step,maxTau;
    alignmentFunction function;
    float* shiftedTrace;
    float* refTrace;
    float samplingFreq,cipherTime;
    string plaintext;
};
#endif
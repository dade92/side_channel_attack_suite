#include<iostream>
#include<vector>
#include<fftw3.h>
#include"config.hpp"
#include"../common/input.hpp"
using namespace std;

class Transform {
public:
    Transform(Config& config,Input& input,int traceSize,float** dataMatrix);
    void computeFilter();
    void padTraces();
    void filterTraces();
    
private:
    float* filterFunction;
    float** dataMatrix;
    padding pad;
    vector<window> windows;
    fftwf_complex* transformation;
    fftwf_complex* filter;
    int traceSize,samplesPerTrace,step;
};
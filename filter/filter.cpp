#include<iostream>
#include<cstdint>
#include<vector>
#include"config.hpp"
#include"transform.hpp"
#include"../common/output.hpp"
#include"../common/input.hpp"
#include"../common/math.hpp"

using namespace std;

/**
 * Filter tool: try to
 * remove noise from traces
 */

int main(int argc,char*argv[]) {
    int i=0;
    if(argc<2) {
        cout<<"Config filename missing. Usage:"<<endl
        <<"./filter.out configFilename"<<endl;
        exit(0);
    }
    Config config(argv[1]);
    config.init();
    Input input(config.filename);
    input.readHeader();
    if(input.numTraces%config.step!=0) {
        cout<<"Step size should be a multiple of the number of traces."<<endl;
        exit(0);
    }
    //this matrix contains step power traces
    float** traceMatrix=new float*[config.step];
    //this matrix contains step plaintext
    uint8_t** plaintext=new uint8_t*[config.step];
    //derive the real size of the traces (power of 2 is better for DFT)
    int traceSize=(input.samplesPerTrace);
    Transform transform(config,input,traceSize,traceMatrix);
    //init data matrix
    for(int w=0;w<config.step;w++) {
        traceMatrix[w]=new float[traceSize];
        plaintext[w]=new uint8_t[input.plainLength];
    }
    Output output(config.outputFilename,config.step,input,traceMatrix,plaintext);
    //for every batch of traces, apply the filter and save on HD
    output.writeHeader();
    //generate the filter window, storing in RAM
    if(config.filterFile.empty()) {
        cout<<"No adaptive filter, computing the windows."<<endl;
        transform.computeFilter();
    }
    else {
        cout<<"adaptive filter passed, computing using .dat file."<<endl;
        transform.computeFilter(config.filterFile);
    }
    while(i<input.numTraces) {
        int read=input.readData(traceMatrix,plaintext,config.step);
        cout<<"Read "<<read<<" elements."<<endl;
//         transform.padTraces();
//         cout<<"Traces padded. Applying filter.."<<endl;
        transform.filterTraces();
        cout<<"Filter applied."<<endl;
        output.writeTraces();
        cout<<"Batch stored."<<endl;
        i+=config.step;
    }
    return 0;
}
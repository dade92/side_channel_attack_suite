#include<iostream>
#include<cstdint>
#include"../common/input.hpp"
#include"../common/output.hpp"
#include"config.hpp"
#include"realigner.hpp"

using namespace std;

int main(int argc,char*argv[]) {
    if(argc<2) {
        cout<<"Usage trace_aligner.out configFile."<<endl;
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
    float** data=new float*[config.step];
    uint8_t** plain=new uint8_t*[config.step];
    for(int w=0;w<config.step;w++) {
        data[w]=new float[input.samplesPerTrace];
        plain[w]=new uint8_t[input.samplesPerTrace];
    }
    int i=0;
    //data matrix is modified by the Realigner
    Output output(config.outputFilename,config.step,input,data,plain);
    output.writeHeader();
    //first batch is outside to initialize the reference trace
    input.readData(data,plain,config.step);
    Realigner realigner(config,input,data[0]);
    cout<<"Aligning batch number "<<0<<endl;
    realigner.alignTraces(data);
    output.writeTraces();
    i+=config.step;
    while(i<input.numTraces) {
        input.readData(data,plain,config.step);
        cout<<"Aligning batch number "<<i/config.step<<endl;
        realigner.alignTraces(data);
        output.writeTraces();
        i+=config.step;
    }
    return 0;
}
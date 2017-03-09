#include<iostream>
#include<cstdint>
#include<string>
#include <fstream>
#include"../common/input.hpp"
#include"../common/output.hpp"
#include"../common/math.hpp"
#include"config.hpp"
#include"realigner.hpp"
#include"traceSplitter.hpp"
/**
 * Tool that alignes traces or splits
 * them using cross correlation
 */
using namespace std;

void showCorrelation(float*corr,Input& input);
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
    uint8_t**plain;
    if(config.m==multiple) {
        if(input.numTraces<=1) {
            cout<<"Multiple mode should be used with different traces."<<endl;
            exit(0);
        }
        cout<<"Multiple mode selected, realigning traces"<<endl;
        float** data=new float*[config.step];
        plain=new uint8_t*[config.step];
        for(int w=0;w<config.step;w++) {
            data[w]=new float[2*input.samplesPerTrace-1];
            plain[w]=new uint8_t[input.plainLength];
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
    } else if(config.m==single) {
        cout<<"Single mode selected, computing auto correlation"<<endl;
        float** data=new float*[1];
        //trace of 2N-1 samples
        data[0]=new float[2*input.samplesPerTrace-1];
        plain=new uint8_t*[1];
        plain[0]=new uint8_t[input.plainLength];
        cout<<"Reading data.."<<endl;
        input.readData(data,plain,1);
        for(int w=input.samplesPerTrace;w<2*input.samplesPerTrace-1;w++)
            data[0][w]=0;
        float *correlation=autoCorrelate(data[0],input.samplesPerTrace);
        if(config.printCorrelation)
            showCorrelation(correlation,input);
        cout<<"Splitting the trace.."<<endl;
        Input originalInput(config.originalFilename);
        originalInput.readHeader();
        if(originalInput.samplesPerTrace!=input.samplesPerTrace) {
            cout<<"files with different length. Are you sure that the file used "
            <<"to derive the correlation is correct?"<<endl;
            exit(0);
        }
        float** originalData=new float*[1];
        originalData[0]=new float[originalInput.samplesPerTrace];
        originalInput.readData(originalData,plain,config.step);
        TraceSplitter traceSplitter(config,originalInput);
        traceSplitter.splitTrace(correlation,originalData);
        cout<<"Splitting ended, traces saved"<<endl;
    }
    return 0;
}

void showCorrelation(float*corr,Input& input) {  
    string datName="datCorr";
    string scriptName="correlation";
    std::ofstream outputScript,outputDat,outputPScript,outputPDat;
    
    outputScript.open(scriptName+".gpl");
    if(outputScript.fail()) {
        cerr << "Please provide a correct output script filename" << endl;
        exit(0);
    }
    outputDat.open(datName+".dat");
    if(outputDat.fail()) {
        cerr << "Please provide a correct output dat filename" << endl;
        exit(0);
    }
    for(int i=0;i<2 * input.samplesPerTrace - 1;i++) {
	outputDat<<i<<" ";
	outputDat<<corr[i]<<endl;
    }
    outputScript << "set term png size "<<1280<<","<<850<<endl;
    outputScript << "set output \""<< "correlation.png\";" << endl;
    outputScript << "set autoscale;" << endl;
    outputScript << "unset key;" << endl;
//     outputScript << "set xtics 5000" <<endl;
    outputScript << "set grid" <<endl;
//     outputScript << "set xrange [0:100000]"<<endl;
    outputScript << "set xlabel \"Tau\" font \",20\";" << endl;
    outputScript << "set ylabel \"correlation\" font \",20\";" << endl << endl;
    outputScript << "plot \""<<datName<<".dat\" with lines linecolor black"<<endl;
}
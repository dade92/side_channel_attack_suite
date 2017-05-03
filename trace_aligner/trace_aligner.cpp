#include<iostream>
#include<cstdint>
#include<string>
#include <fstream>
#include<vector>
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

void showCorrelation(float*corr,Input& input,Config&c);
int main(int argc,char*argv[]) {
    if(argc<2) {
        cout<<"Usage:\n ./trace_aligner.out configFile."<<endl;
        exit(0);
    }
    Config config(argv[1]);
    config.init();
    uint8_t**plain;
    float** data=new float*[config.step];
    plain=new uint8_t*[config.step];
    //dummy initialization, I don't know these numbers yet
    Output output(config.outputFilename,config.step,'f',0,0,0,data,plain);
    //fake header, is repaired at the end
    output.writeHeader();
    vector<string>::iterator iterator;
    int fileIndex=0,totTraces=0,numSamples;
    bool first=true;
    for(iterator=config.filenames.begin();iterator!=config.filenames.end();++iterator) {
        Input input(*iterator);
        input.readHeader();
        for(int w=0;w<config.step;w++) {
            data[w]=new float[2*input.samplesPerTrace-1];
            plain[w]=new uint8_t[input.plainLength];
        }
        if(first) {
            numSamples=input.samplesPerTrace;
            output.setPlainLength(input.plainLength);
            if(input.numTraces%config.step!=0) {
                cout<<"Step size should be a multiple of the number of traces."<<endl;
                exit(0);
            }
            if(config.maxTau>input.samplesPerTrace) {
                cout<<"Wrong maxTau."<<endl;
                exit(0);
            }
        }
        int i=0;
        if(config.m==multiple) {
            totTraces+=input.numTraces;
            if(input.numTraces<=1 || config.step<=1) {
                cout<<"Multiple mode should be used with different traces."<<endl;
                exit(0);
            }
            if(first) {
                output.setNumOfSamples(input.samplesPerTrace);
                output.setNumOfTraces(input.numTraces);
                first=false;
            }
            cout<<"Multiple mode selected, realigning traces"<<endl;
            //data matrix is modified by the Realigner
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
            //here I still don't know how many traces I have, neither how many samples per each trace
            cout<<"Single mode selected, computing auto correlation"<<endl;
            //trace of 2N-1 samples
            data[0]=new float[2*input.samplesPerTrace-1];
            //opens the file that is actually split
            Input originalInput(config.originalFilenames[fileIndex]);
            originalInput.readHeader();
            if(originalInput.samplesPerTrace!=input.samplesPerTrace) {
                cout<<"files with different length. Are you sure that the file used "
                <<"to derive the correlation is correct?"<<endl;
                exit(0);
            }
            float** originalData=new float*[1];
            originalData[0]=new float[originalInput.samplesPerTrace];
            while(i<input.numTraces) {
                cout<<"Reading data.."<<endl;
                input.readData(data,plain,1);
                for(int w=input.samplesPerTrace;w<2*input.samplesPerTrace-1;w++)
                    data[0][w]=0;
                float *correlation=autoCorrelate(data[0],input.samplesPerTrace);
                if(first) {
                    if(config.printCorrelation)
                        showCorrelation(correlation,input,config);
                }
                cout<<"Splitting the trace.."<<endl;
                originalInput.readData(originalData,plain,1);
                TraceSplitter traceSplitter(config,originalInput,plain);
                //set again the number of samples, using the length of the cipher
                totTraces+=traceSplitter.splitTrace(correlation,originalData,output,numSamples,first);
                i++;
                first=false;
            }
            cout<<"Splitting ended, trace number "<<fileIndex+1<<" saved"<<endl;
            delete originalData[0];
//             delete originalData[0];
//             delete data[0];
        }
        fileIndex++;
        for(int w=0;w<config.step;w++) {
            delete data[w];
            delete plain[w];
        }
    }
    cout<<"Writing the new header"<<endl;
    //write the real header, with the total number of samples written
    output.setNumOfSamples(numSamples);
    output.setNumOfTraces(totTraces);
    output.rewriteHeader();
    return 0;
}

void showCorrelation(float*corr,Input& input,Config& c) {  
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
    for(int i=0;i<2 * input.samplesPerTrace - 1;i+=20) {
	outputDat<<((i-input.samplesPerTrace)/c.samplingFreq)*1000000<<" ";
	outputDat<<corr[i]<<endl;
    }
    outputScript << "set term png size "<<1280<<","<<850<<endl;
    outputScript << "set output \""<< "correlation.png\";" << endl;
    outputScript << "set autoscale;" << endl;
    outputScript << "unset key;" << endl;
//     outputScript << "set xtics 5000" <<endl;
    outputScript << "set grid" <<endl;
//     outputScript << "set xrange [0:100000]"<<endl;
    outputScript << "set xlabel \"Tau[us]\" font \",20\";" << endl;
    outputScript << "set ylabel \"correlation\" font \",20\";" << endl << endl;
    outputScript << "plot \""<<datName<<".dat\" with lines linecolor black"<<endl;
    
    outputScript << "set terminal epslatex size "<<5.65<<","<<2<<endl;
    outputScript << "set output \""<< "correlation.tex\";" << endl;
    outputScript << "set autoscale;" << endl;
    outputScript << "unset key;" << endl;
//     outputScript << "set xtics 5000" <<endl;
    outputScript << "set grid" <<endl;
//     outputScript << "set xrange [0:100000]"<<endl;
    outputScript << "set xlabel \"Tau[us]\" font \",20\";" << endl;
    outputScript << "set ylabel \"correlation\" font \",20\";" << endl << endl;
    outputScript << "plot \""<<datName<<".dat\" with lines linecolor black"<<endl;
}

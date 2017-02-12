#include"output.hpp"

Output::Output(string outputFile,int s,Input& input,float** data,uint8_t**plains) {
    dataMatrix=data;
    plaintext=plains;
    filename=outputFile.c_str();
    samplesPerTrace=input.samplesPerTrace;
    numTraces=input.numTraces;
    step=s;
    format=input.format;
    plainLength=input.plainLength;
    fp=fopen(filename,"wb");
    if(fp==NULL) {
        cout<<"Can't open output file."<<endl;
        exit(0);
    }
}

Output::~Output() {
    fclose(fp);
}

Output::Output(string outputFile,int step,
           int numTraces,int samplesPerTrace,
           int plainLength,float**data,uint8_t**plains) {
    dataMatrix=data;
    plaintext=plains;
    filename=outputFile.c_str();
    this->samplesPerTrace=samplesPerTrace;
    this->numTraces=numTraces;
    this->step=step;
    this->plainLength=plainLength;
    format='f';
    fp=fopen(filename,"wb");
    if(fp==NULL) {
        cout<<"Can't open output file."<<endl;
        exit(0);
    }
}

void Output::writeHeader() {
    fwrite(&numTraces, sizeof(uint32_t), 1, fp);
    fwrite(&samplesPerTrace, sizeof(uint32_t), 1, fp);
    fwrite(&format, sizeof(uint8_t), 1, fp);
    fwrite(&plainLength, sizeof(uint8_t), 1, fp);
}

void Output::writeTraces() {
    for(int i=0;i<step;i++) {
        fwrite(dataMatrix[i],sizeof(float),samplesPerTrace,fp);
        fwrite(plaintext[i],sizeof(uint8_t),plainLength,fp);
    }
}

void Output::rewindFile() {
    fseek(fp,0,SEEK_SET);
}

void Output::setNumOfTraces(int numTraces) {
    this->numTraces=numTraces;
}
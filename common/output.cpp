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

Output::Output(string outputFile,int s,Input& input,double** data,uint8_t**plains) {
    dataMatrix2=data;
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

Output::Output(string outputFile,int step,char format,
           int numTraces,int samplesPerTrace,
           int plainLength,float**data,uint8_t**plains) {
    dataMatrix=data;
    plaintext=plains;
    filename=outputFile.c_str();
    this->samplesPerTrace=samplesPerTrace;
    this->numTraces=numTraces;
    this->step=step;
    this->plainLength=plainLength;
    this->format=format;
    fp=fopen(filename,"wb");
    if(fp==NULL) {
        cout<<"Can't open output file."<<endl;
        exit(0);
    }
}

Output::Output(string outputFile,int step,char format,
           int numTraces,int samplesPerTrace,
           int plainLength,double**data,uint8_t**plains) {
    dataMatrix2=data;
    plaintext=plains;
    filename=outputFile.c_str();
    this->samplesPerTrace=samplesPerTrace;
    this->numTraces=numTraces;
    this->step=step;
    this->plainLength=plainLength;
    this->format=format;
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
    if(format==SAMPLE_FORMAT_FLOAT) {
        for(int i=0;i<step;i++) {
            fwrite(dataMatrix[i],sizeof(float),samplesPerTrace,fp);
            fwrite(plaintext[i],sizeof(uint8_t),plainLength,fp);
        }
    }
    else if(format==SAMPLE_FORMAT_DOUBLE) {
        for(int i=0;i<step;i++) {
            fwrite(dataMatrix2[i],sizeof(double),samplesPerTrace,fp);
            fwrite(plaintext[i],sizeof(uint8_t),plainLength,fp);
        }    
    }
}

void Output::rewindFile() {
    fseek(fp,0,SEEK_SET);
}

void Output::setNumOfTraces(uint32_t numTraces) {
    this->numTraces=numTraces;
}
void Output::setNumOfSamples(uint32_t numSamples) {
    this->samplesPerTrace=numSamples;
}
void Output::setPlainLength(uint8_t pl) {
    this->plainLength=pl;
}
void Output::setDataBuffer(float**buffer) {
    dataMatrix=buffer;
}
void Output::setPlainBuffer(uint8_t**p) {
    plaintext=p;
}
void Output::rewriteHeader() {
    this->rewindFile();
    fwrite(&numTraces, sizeof(uint32_t), 1, fp);
    fwrite(&samplesPerTrace, sizeof(uint32_t), 1, fp);
    fwrite(&format, sizeof(uint8_t), 1, fp);
    fwrite(&plainLength, sizeof(uint8_t), 1, fp);
}
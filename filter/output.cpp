#include"output.hpp"

Output::Output(Config& config,Input& input,float** data,uint8_t**plains) {
    dataMatrix=data;
    plaintext=plains;
    filename=config.outputFilename.c_str();
    samplesPerTrace=input.samplesPerTrace;
    numTraces=input.numTraces;
    step=config.step;
    format=input.format;
    plainLength=input.plainLength;
    fp=fopen(filename,"wb");
    if(fp==NULL) {
        cout<<"Error during file opening."<<endl;
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
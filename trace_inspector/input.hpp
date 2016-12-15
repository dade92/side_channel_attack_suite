#ifndef INPUT_H_
#define INPUT_H_
#include<stdio.h>
#include<string>
#include<iostream>
#include<cstdint>
#define SAMPLE_FORMAT_FLOAT 'f'
#define SAMPLE_FORMAT_DOUBLE 'd'
#define SAMPLE_FORMAT_CHAR 'c'
using namespace std;
/**
 * class that reads the .dat file, stores header information
 * and contains helper methods to read the traces.
 * IMPORTANT: after calling the constructor, is compulsory
 * to call also the readHeader method that will read file 
 * information
 * 
 */
class Input {
public:
    Input(std::string filename);
    ~Input();
    void readHeader();
    void readPlaintext(int index,char*plains,int step);
    int readData(float** dataMatrix,uint8_t** plains,int step);
    void rewind_file();
    void printInformation();
    int samplesPerTrace;
    int numTraces;
    short dataDimension;
    char format;
    char plainLength;
private:
    FILE* file;
    long fileSize;
    int getTraceOffset(int traceIndex);
    int getPlainOffset(int plainIndex);
};

#endif
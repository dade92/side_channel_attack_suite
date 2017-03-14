#include "input.hpp"
Input::Input(string filename) {
    file=fopen(filename.c_str(),"r");
    if(file==NULL) {
        cout<<"Can't open .dat file."<<endl;
        exit(0);
    }
}

void Input::readHeader() {
    fseek (file,0,SEEK_END);
    fileSize=ftell(file);
    rewind (file);
    /*
     * read the header: 10 bytes that represent the total number of traces
     * the samples per trace the format of the numbers stored 
     * and the plain length
     */
    fread(&numTraces,sizeof(uint32_t),1,file);
    fread(&samplesPerTrace,sizeof(uint32_t),1,file);
    fread(&format,sizeof(uint8_t),1,file);
    fread(&plainLength,sizeof(uint8_t),1,file);
    this->printInformation();
    switch(format) {   
        case SAMPLE_FORMAT_FLOAT:
            dataDimension=sizeof(float);
            break;
        case SAMPLE_FORMAT_DOUBLE:
            dataDimension=sizeof(double);
            break;
        case SAMPLE_FORMAT_CHAR:
            dataDimension=sizeof(char);
            break;
        default:
            cout<<"Invalid format type."<<endl;
            exit(0);
    }
    long long plainSize=plainLength*numTraces;
    long long traceSize=(long long)numTraces*samplesPerTrace*dataDimension;
    if(10+plainSize+traceSize!=fileSize) {
        cout<<"Incorrect file size: it should be "<<10+plainSize+traceSize<<" bytes but it is "<<fileSize<<" bytes"<<endl;
        exit(0);
    }
    temp=NULL;
}

void Input::rewind_file() {
    rewind(file);
    fread(&numTraces,sizeof(uint32_t),1,file);
    fread(&samplesPerTrace,sizeof(uint32_t),1,file);
    fread(&format,sizeof(uint8_t),1,file);
    fread(&plainLength,sizeof(uint8_t),1,file);
}
//this can be avoided
void Input::readPlaintext(int index,char*plains,int step) {
    int offset=this->getPlainOffset(index);
    fseek(file,offset,SEEK_SET);
    fread(plains,sizeof(char),plainLength,file);
}
//reads incrementally all data, both the trace and the plaintext.returns the number of read bytes
int Input::readData(float** dataMatrix,uint8_t** plains,int step) {
    //takes all the trace length
    int r=0;
    for(int i=0;i<step;i++) {
        r+=fread(dataMatrix[i],dataDimension,samplesPerTrace,file);
        //always char type
        fread(plains[i],sizeof(uint8_t),plainLength,file);
    }
    return r;
}
//reads incrementally all data, both the trace and the plaintext.returns the number of read bytes
int Input::readData(double** dataMatrix,uint8_t** plains,int step) {
    //takes all the trace length
    int r=0;
    for(int i=0;i<step;i++) {
        r+=fread(dataMatrix[i],dataDimension,samplesPerTrace,file);
        //always char type
        fread(plains[i],sizeof(uint8_t),plainLength,file);
    }
    return r;
}

Input::~Input() {
    fclose(file);
}
void Input::printInformation() {
    cout<<"File size: "<<fileSize<<" bytes"<<endl;
    cout<<"Num of traces: "<<numTraces<<endl;
    cout<<"Samples per each trace: "<<samplesPerTrace<<endl;
    cout<<"Format of data: "<<format<<endl;
    cout<<"Plain length: "<<(int)plainLength<<endl;
}

int Input::getTraceOffset(int traceIndex) {
    return 10+traceIndex*(samplesPerTrace*dataDimension+plainLength);
}

int Input::getPlainOffset(int plainIndex) {
    return 10+plainIndex*(samplesPerTrace*dataDimension)+samplesPerTrace*dataDimension;    
}
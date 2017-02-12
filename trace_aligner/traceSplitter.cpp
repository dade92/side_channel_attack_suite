#include"traceSplitter.hpp"

TraceSplitter::TraceSplitter(Config& c,Input& i) {
    cipherTime=c.cipherTime;
    samplingFreq=c.samplingFreq;
    samplesPerTrace=i.samplesPerTrace;
    startPlain=new uint8_t[c.startPlain.size()/2];
    key=new uint8_t[c.key.size()/2];
    int n=0;
    string temp,temp2;
    for(int i=0;i<c.startPlain.size()/2;i++) {
        temp=temp2="";
        temp+=c.startPlain[n];
        temp+=c.startPlain[n+1];
        temp2+=c.key[n];
        temp2+=c.key[n+1];
        startPlain[i]=strtol(temp.c_str(),NULL,16);
        key[i]=strtol(temp2.c_str(),NULL,16);
        n+=2;
    }
    outputFilename=c.outputFilename;
    plainLength=i.plainLength;
}
//TODO:trace is not necessary probably
void TraceSplitter::splitTrace(float*correlation,float**data) {  
    int n,i;
    int length=cipherTime*samplingFreq;
    int delayIndex,numTraces=1;
    float**trace=new float*[1];
    trace[0]=new float[length];
    uint8_t**plains=new uint8_t*[1];
    plains[0]=new uint8_t[plainLength];
    AES aes(key,plainLength*8,AES_ENCRYPT);
    aes.encrypt(plains[0],plains[0]);
    for(n=0;n<length;n++)
        trace[0][n]=data[0][n];
    Output output(outputFilename,1,0,length,plainLength,trace,plains);
    output.writeHeader();
    output.writeTraces();
    for(int w=length/2;w<samplesPerTrace-length;w+=length) {
        i=0;
        delayIndex=findMaxIndex(correlation,w,w+length);
        aes.encrypt(plains[0],plains[0]);
        //TODO:put the trace correctly
        for(n=delayIndex;n<delayIndex+length;n++) {
            trace[0][i]=data[0][n];
            i++;
        }
        output.writeTraces();
        numTraces++;
    }
    //to correct the header
    output.rewindFile();
    output.setNumOfTraces(numTraces);
    output.writeHeader();
}

int TraceSplitter::findMaxIndex(float*correlation,int start,int end) {
    int maxIndex=0;
    int max=0;
    for(int i=start;i<end;i++) {
        if(correlation[i]>max) {
            max=correlation[i];
            maxIndex=i;
        }
    }
    return maxIndex;
}
#include"traceSplitter.hpp"

TraceSplitter::TraceSplitter(Config& c,Input& in,uint8_t**p) {
    cipherTime=c.cipherTime;
    samplingFreq=c.samplingFreq;
    samplesPerTrace=in.samplesPerTrace;
    startPlain=p[0];
    key=new uint8_t[c.key.size()/2];
    int n=0;
    string temp,temp2;
    for(int i=0;i<in.plainLength;i++) {
        temp=temp2="";
        temp2+=c.key[n];
        temp2+=c.key[n+1];
        key[i]=strtol(temp2.c_str(),NULL,16);
        n+=2;
    }
    outputFilename=c.outputFilename;
    plainLength=in.plainLength;
}
/**
 * given a long trace (pointed by data)
 * and its auto correlation array, splits
 * the trace into sub traces, based on its
 * auto correlation
 */
int TraceSplitter::splitTrace(float*correlation,float**data,
                              Output& output,int& numSamples,bool& first) {
    int n,i,delayIndex;
    int length=cipherTime*samplingFreq;
    int startS;
    if(first) {
        cout<<"trying to derive the cipher length from sample "<<samplesPerTrace+length/2<<" to sample "<<samplesPerTrace+length/       2+length<<endl;
        startS=findMaxIndex(correlation,samplesPerTrace-length/2,samplesPerTrace+length/2);
        length=findMaxIndex(correlation,samplesPerTrace+length/2,samplesPerTrace+length/2+length)-startS;
        cout<<"Detected cipher length:"<<length<<endl;
        numSamples=length;
    }
    float**trace=new float*[1];
    trace[0]=new float[length];
    uint8_t**plains=new uint8_t*[1];
    plains[0]=new uint8_t[plainLength];
    for(n=0;n<plainLength;n++)
        plains[0][n]=startPlain[n];
    /*for(int x=0;x<16;x++)
        printf("0x%x ",plains[0][x]);*/
    printf("\n");
    for(n=startS-samplesPerTrace;n<length+startS-samplesPerTrace;n++)
        trace[0][n]=data[0][n];
    //needed, otherwise output class will not write anything on disk
    if(first) {
        output.setNumOfTraces(samplesPerTrace/length+1);
        output.setNumOfSamples(length);
        output.setPlainLength(plainLength);
    }
    //set the new buffers
    output.setDataBuffer(trace);
    output.setPlainBuffer(plains);
    output.writeTraces();
    AES aes(key,plainLength*8,AES_ENCRYPT);
    int traceCount=1;
    for(int w=length+startS;w<2*samplesPerTrace-length;w=delayIndex) {
        traceCount++;
        if(traceCount<=20) {
            for(int x=0;x<16;x++)
                printf("%x ",plains[0][x]);
            printf("\n");
        }
        aes.encrypt(plains[0],plains[0]);
        i=0;
        delayIndex=findMaxIndex(correlation,w+length/2,(int)(w+length*1.5));
        for(n=w-samplesPerTrace;n<delayIndex-samplesPerTrace;n++) {
            trace[0][i]=data[0][n];
            i++;
        }
        output.writeTraces();
    }
    return traceCount;
}
/**
 * Finds the index of the local
 * maximum
 */
int TraceSplitter::findMaxIndex(float*correlation,int start,int end) {
    int maxIndex=start;
    float max=correlation[start];
    for(int i=start;i<end;i++) {
        if(correlation[i]>max) {
            max=correlation[i];
            maxIndex=i;
        }
    }
    return maxIndex;
}
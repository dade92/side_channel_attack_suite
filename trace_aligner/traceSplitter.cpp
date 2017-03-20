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
/**
 * given a long trace (pointed by data)
 * and its auto correlation array, splits
 * the trace into sub traces, based on its
 * auto correlation
 */
void TraceSplitter::splitTrace(float*correlation,float**data) {
    int n,i;
    int length=cipherTime*samplingFreq;
    cout<<"trying to derive the cipher length from sample "<<samplesPerTrace+length/2<<" to sample "<<samplesPerTrace+length/2+length<<endl;
    length=findMaxIndex(correlation,samplesPerTrace+length/2,samplesPerTrace+length/2+length)-samplesPerTrace;
    cout<<"Detected cipher length:"<<length<<endl;
    int delayIndex;
    float**trace=new float*[1];
    trace[0]=new float[length];
    uint8_t**plains=new uint8_t*[1];
    plains[0]=new uint8_t[plainLength];
    for(n=0;n<plainLength;n++)
        plains[0][n]=startPlain[n];
    AES aes(key,plainLength*8,AES_ENCRYPT);
    for(n=0;n<length;n++)
        trace[0][n]=data[0][n];
    Output output(outputFilename,1,'f',samplesPerTrace/length+1,length,plainLength,trace,plains);
    output.writeHeader();
    output.writeTraces();
//     aes.encrypt(plains[0],plains[0]);
    /*cout<<"Second plain:"<<endl;
    for(int x=0;x<16;x++)
        printf("0x%x ",plains[0][x]);*/
    for(int w=samplesPerTrace+length/2;w<2*samplesPerTrace-length;w+=length) {
        i=0;
        delayIndex=findMaxIndex(correlation,w,w+length)-samplesPerTrace;
        for(n=delayIndex;n<delayIndex+length;n++) {
            trace[0][i]=data[0][n];
            i++;
        }
        output.writeTraces();
        aes.encrypt(plains[0],plains[0]);
    }
}

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
#include <stdio.h>
#include <iostream>
#include <stdio.h>
#include <cstdint>
#include "config.hpp"
#include "../common/input.hpp"
#include "powermodel.hpp"
#include "output.hpp"
#include "statisticalAnalysis.hpp"
//max RAM allower to occupy in MB
#define MAX_RAM 300 
/**
 * tool that performs a CPA attack and
 * a known key analysis
 */
using namespace std;

int main(int argc,char* argv[]) {
    int i=0,intervalIndex,countStep=0;
    if(argc<2) {
        cout<<"Config filename missing. Usage:"<<endl<<"./analysis configFilename"<<endl;
        exit(0);
    }
    string configFilename(argv[1]);
    Config config(configFilename);
    //init the configuration class that contains all the params like filename,step etc..
    config.init();
    //opens the .dat file and read header information
    Input input(config.filename);
    input.readHeader();
    //little constraint
    if(input.numTraces%config.step!=0) {
        cout<<"Step should be a divisor of the number of traces."<<endl;
        exit(0);
    }
    vector<interval>::iterator iterator;
    //check if all the intervals are correctly setup before starting analysis
    for(iterator=config.intervals.begin();iterator!=config.intervals.end();++iterator) {
        if(iterator->end>input.samplesPerTrace) {
            cout<<iterator->name<<":Wrong end sample in the interval section."<<endl;
            exit(0);
        } else if(iterator->sbox>input.plainLength-1) { //attacking an sbox greater than the plaintext size
            cout<<iterator->name<<":Cannot use the "<<iterator->sbox<<"th sbox because plaintext is shorter."<<endl;
            exit(0);            
        } else if(iterator->keySize<0 || iterator->intSize<0 
            || iterator->intSize%8!=0) { //invalid intermediate or key sizes
            cout<<iterator->name<<":Wrong key/intermediate size."<<endl;
            exit(0);
        } else if(iterator->keySize==0 && (iterator->position.compare("ar")==0 ||
                                           iterator->position.compare("sub")==0) ||
                                           iterator->position.compare("sr")==0 || 
                                           iterator->position.compare("mc")==0) {
            //AES model setup can't be used if keySize is 0 (=known input attack)
            cout<<iterator->name<<":Known key attack using an AES position."<<endl;
            exit(0);
        }
    }
    //check on the occupied RAM
    if(config.step*input.samplesPerTrace*input.dataDimension/(1024*1024) > MAX_RAM) {
        cout<<"Too much RAM required."<<endl;
        exit(0);
    }
    
    //this matrix contains step power traces
    float** traceMatrix=new float*[config.step];
    //this matrix contains step plaintext
    uint8_t** plaintext=new uint8_t*[config.step];
    //this matrix contains power predictions. size depends on the intervals,init after
    unsigned** pm;
    //this matrix contains the pearson correlation coefficient (every key, in time).
    //its size depends on each interval,init after
    float** pearsonMatrix;
    
    /*
     * for each interval (vector size==#intervals), 
     * I have a sequence of result values, one for each step
     */
    vector<result*> results;
    /*
     * for each interval (vector size==#intervals) 
     * I need to store temp mean and variance 
     * of the power trace and of the power predictions
     * in order to compute the correlation
     */
    vector<statistic*> traceStatistics;
    vector<statistic*> powerStatistic;
    //temp covariance matrix, one for each interval;
    vector<float**> m12;
    //for each interval (vector size==#intervals) I store the final Pearson coefficients
    vector<float**>finalPearson;
    
    for(unsigned w=0;w<config.intervals.size();w++) {
        int keySpace=pow(2,config.intervals[w].keySize);
        //init the vector of the results for each interval
        //every interval has an array of #step results
        result* r=new result[(int)input.numTraces/config.step];
        statistic* s=new statistic[keySpace];
        for(int x=0;x<keySpace;x++) {
            s[x].mean=s[x].m2=0;
        }
        statistic* s2=new statistic[config.intervals[w].end-config.intervals[w].start];
        for(int x=0;x<config.intervals[w].end-config.intervals[w].start;x++) {
            s2[x].mean=s2[x].m2=0;
        }
        //init vectors
        results.push_back(r);
        finalPearson.push_back(new float*[keySpace]);
        powerStatistic.push_back(s);
        traceStatistics.push_back(s2);
        m12.push_back(new float*[keySpace]);
        for(int n=0;n<keySpace;n++)
            m12[w][n]=new float[config.intervals[w].end-config.intervals[w].start];
    }
    //for each interval, init the covariance matrix
    for(unsigned w=0;w<config.intervals.size();w++) {
        for(int x=0;x<pow(2,config.intervals[w].keySize);x++) {
            for(int y=0;y<config.intervals[w].end-config.intervals[w].start;y++)
                m12[w][x][y]=0;
        }
    }
    //init matrix
    for(int w=0;w<config.step;w++) {
        traceMatrix[w]=new float[input.samplesPerTrace];
        plaintext[w]=new uint8_t[input.plainLength];
    }
    int read;
    /* ************ start of the analysis ************ */
    while(i!=input.numTraces) {
        intervalIndex=0;
        i+=config.step;
        cout<<endl<<"**************************"<<endl;
        cout<<"Analysis with "<<i<<" traces"<<endl;
        cout     << "**************************"<<endl<<endl;
        //reads incrementally, the file is passed once, reading step traces every time
        read=input.readData(traceMatrix,plaintext,config.step);
        cout<<"-Read "<<read<<" bytes"<<endl;
        vector<interval>::iterator it;
        int keySpace;
        for(it=config.intervals.begin();it!=config.intervals.end();++it) {
            keySpace=pow(2,it->keySize);
            cout<<"-Starting analysis on interval "<<it->name<<endl;
            //for every interval I init a new matrix of power traces, #rows=step, #columns=keySpace
            pm=new unsigned*[config.step];
            for(int x=0;x<config.step;x++)
                pm[x]=new unsigned[keySpace];
            //generate the model. I have to do for each interval because every interval has its own model
            PowerModel powerModel(*it,config);
            powerModel.generate(plaintext,pm);
            cout<<"--Power model DONE"<<endl;
            //for every interval I init a new matrix of pearson coefficients, #rows=keySpace #columns=end-start
            pearsonMatrix=new float*[keySpace];
            for(int w=0;w<keySpace;w++) 
                pearsonMatrix[w]=new float[it->end-it->start];
            //in order to compute the correlation, I need temp values
            Pearson pearson(*it,config,traceStatistics[intervalIndex],
                            powerStatistic[intervalIndex],m12[intervalIndex]);
            //computes correlation and stores results in pearsonMatrix
            pearson.correlate(i,traceMatrix,pm,pearsonMatrix);
            cout<<"--Correlation analysis DONE"<<endl;
            //finds the maximum values and put in the correct results position
            pearson.findMax(pearsonMatrix,results[intervalIndex][countStep],it->key,keySpace,it->end-it->start);
            //compute the confidence intervals and stores them in the correct results position
            pearson.computeConfidence(i,results[intervalIndex][countStep]);
            cout<<"--Analysis ended, intermediate results saved"<<endl;
            //if we are in the final step,store the matrix of the key coefficients
            if(i==input.numTraces) {
                cout<<"Storing last results.."<<endl;
                //init the matrix that stores the final pearson matrix
                for(int w=0;w<keySpace;w++)
                    finalPearson[intervalIndex][w]=new float[it->end-it->start];
                //copy the temp matrix inside the finalPearson matrix
                for(int x=0;x<keySpace;x++) {
                    for(int y=0;y<it->end-it->start;y++)
                        finalPearson[intervalIndex][x][y]=pearsonMatrix[x][y];
                }
            }
            intervalIndex++;
            //free these matrix, avoid memory leaks
            for(int x=0;x<config.step;x++)
                delete[] pm[x];
            for(int x=0;x<keySpace;x++)
                delete[] pearsonMatrix[x];
        }
        countStep++;
    }
    cout<<"Writing final results.."<<endl;
    Output output(config,input);
    //output writes the logs and the gnuplot scripts
    output.writeResults(results,finalPearson);
    //free resources
    for(int w=0;w<config.step;w++) {
        delete[] traceMatrix[w];
        delete[] plaintext[w];
    }
    return 0;
}

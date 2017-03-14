#ifndef OUTPUT_H_
#define OUTPUT_H_
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <math.h>
#include <boost/format.hpp>
#include "../common/input.hpp"
#include "config.hpp"

using namespace std;
/**
 * class that generate the log file, 
 * the gnuplot scripts 
 * and dat file used to plot
 * the results
 */

typedef struct {
    float pearson;
    int bestKey;
    float pearsonCorrect;
    float ic;
    float icCorrect;
}result;

class Output {
public:
    Output(Config& config,Input& input);
    void writeResults(vector<result*>& results,
                      vector<float**>& totalPearson);
private:
    string outputDir,filename;
    int numTraces;
    float figureWidth,figureHeight,xtics;
    int numIntervals,numSteps,step;
    float alpha,samplingFreq,clockFreq,scale;
    Timescale timescale;
    bool grid,bw,abs_value;
    Unit unit;
    string getKeyAsString(int key);
    //vector that contains the intervals
    vector<interval> intervals;
};
#endif

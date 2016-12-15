#ifndef STATISTICAL_ANALISYS_H_
#define STATISTICAL_ANALISYS_H_
#include <string>
#include <vector>
#include <math.h>
#include <thread>
#include <functional>
#include <boost/math/special_functions/erf.hpp>
#include "config.hpp"
#include "output.hpp"

using namespace std;
/**
 * class that perform Pearson correlation
 * between power traces and power predictions
 */

/* contains the online values of the mean and the variance of
 * the power model and the trace
 * 
 */
typedef struct {
    float mean;
    float m2;
}statistic;

class Pearson {
public:
    //take as input the temp statistics for the trace and for the power (mean and variance for each column)
    Pearson(interval& i,Config& c,statistic* ts,statistic* ps,float** m12);
    void correlate(int startNumTrace,float** dataMatrix,unsigned**powerMatrix,float**pearsonCoefficient);
    void findMax(float** matrix,result& r,int k,int row,int col);
    void computeConfidence(int incrementalNumTraces,result& r);
private:
    int startSample,endSample,step,keySpace;
    //temp statistic (mean and m2) for each interval
    statistic* traceStatistics;
    statistic* powerStatistics;
    float** modifiedDataMatrix;
    float** m12;
    float alpha;
    void generateDataMatrix(float** dataMatrix);
    float getConfidence(float r,int numTraces);
    float computeVariance(float**matrix,float mean,int col,int numTraces);
    float computeVariancePower(unsigned**matrix,float mean,int col,int numTraces);
    void computeDataStatistic(float** deltaTrace,int incrementalNumTraces);
    void computePowerStatistic(unsigned** powerMatrix,float** deltaPower,int incrementalNumTraces);
    
};
#endif
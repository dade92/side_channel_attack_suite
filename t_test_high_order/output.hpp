#ifndef OUTPUT_H_
#define OUTPUT_H_
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iomanip>
#include "../common/input.hpp"
#include "config.hpp"

using namespace std;
/**
 * class of t test that generate the log file, 
 * the gnuplot scripts 
 * and dat file used to plot
 * the results
 */


class Output {
public:
    Output(Config& config,Input& input1,Input& input2);
    void writeResults(std::vector<float *>& tStatistics,std::vector<float *>& pStatistics);
private:
    string filename;
    int order,samplesPerTrace,startSample,endSample,xtics;
    bool grid;
    string getKeyAsString(int key);
};
#endif

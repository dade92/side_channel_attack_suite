#ifndef CONFIG_H_
#define CONFIG_H_
#include<iostream>
#include<string>
#include<fstream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/info_parser.hpp>
#include <boost/foreach.hpp>

using namespace std;
using namespace boost::property_tree;
/**
 * Config class for the trace aligner tool,
 * parses the config file and put 
 * the parameters in public attributes
 */

//here add the filter type, then modify configuration parsing
enum alignmentFunction {
    crossCorr
};

enum mode {
    single,multiple
};

class Config {
public:
    Config(string configFile);
    void init();
    string filename,outputFilename,startPlain,key,originalFilename;
    float samplingFreq,cipherTime;
    int step,maxTau;
    alignmentFunction function;
    mode m;
    bool printCorrelation;
    
private:
    string configFile;
    ifstream config;
};
#endif
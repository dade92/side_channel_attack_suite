#ifndef CONFIG_H_
#define CONFIG_H_
#include<string>
#include<fstream>
#include<vector>
#include<cstdlib>
#include<iostream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/info_parser.hpp>
#include <boost/foreach.hpp>

using namespace std;
using namespace boost::property_tree;
/**
 * class of trace inspector that parses the config file and
 * stores all the parameters in public
 * variables that will be used by other
 * classes of the tool.
 * IMPORTANT: after the constructor, call
 * init method to setup the obect
 */

class Config {
    
public:
    Config(string configFile);
    void init();
    string filename;
    int numTraceToPrint;
    int batch,maxSample,startSample;
    bool grid;
    float samplingFreq,clockFreq;    
private:
    string configFile;
    ifstream config;
};
#endif
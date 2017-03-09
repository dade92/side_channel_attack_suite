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
 * class of t test that parses the config file and
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
    string filename2;
    int batch,maxSample,startSample,order,
        maxTau2,maxTau3,maxTau4,maxTau5,xtics;
    float imageWidth,imageHeight;
    bool grid;
    float samplingFreq,clockFreq,alpha;
private:
    string configFile;
    ifstream config;
};
#endif
#ifndef CONFIG_H_
#define CONFIG_H_
#include<iostream>
#include<string>
#include<fstream>
#include<vector>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/info_parser.hpp>
#include <boost/foreach.hpp>

using namespace std;
using namespace boost::property_tree;

enum windowType {
    rect
};
enum filterType {
    lowPass,bandPass,highPass
};
enum padding {
    hold,mean,zero
};

typedef struct   {
    string name;
    filterType type;
    windowType windowFunction;
    float lowFrequency;
    float highFrequency;
    
} window;

class Config {
public:
    Config(string configFile);
    void init();
    string filename,outputFilename;
    float samplingFreq;
    int step;
    padding pad;
    vector<window> windows;
    
private:
    string configFile;
    ifstream config;    
};
#endif
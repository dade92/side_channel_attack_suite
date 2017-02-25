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
/**
 * Config class for the filter tool,
 * parses the config file and put 
 * the parameters in public attributes
 */

//here add the filter type, then modify configuration parsing
enum windowType {
    rect,hann,nuttall
};

enum filterType {
    lowPass,bandPass,highPass
};

enum filterCombination {
    doNothing,normalize,clamp
};
//here adds padding type, then modify configuration parsing
enum padding {
    hold,mean,zero
};
//window struct
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
    string filename,outputFilename,filterFile;
    float samplingFreq;
    int step;
    padding pad;
    bool demodularize;
    filterCombination filterComb;
    vector<window> windows;
    
private:
    string configFile;
    ifstream config;    
};
#endif
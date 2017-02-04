#ifndef GENERATOR_CONFIG_H_
#define GENERATOR_CONFIG_H_
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
 * Config class for the filter generator tool,
 * parses the config file and put
 * the parameters in public attributes
 */

//here adds padding type, then modify configuration parsing
enum padding {
    hold,mean,zero
};

class Config {
public:
    Config(string configFile);
    void init();
    string filename,outputFilename;
    int step;
    padding pad;
    
private:
    string configFile;
    ifstream config;    
};
#endif
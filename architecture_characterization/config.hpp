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
 * class that parses the config file and
 * stores all the parameters in public
 * variables that will be used by other
 * classes of the tool.
 * IMPORTANT: after the constructor, call
 * init method to setup the obect
 */
typedef struct {
        string name;
        int start;
        int end;
        int key;
        string model; 
        int sbox;
        bool bw;
        int keySize;
        int intSize;
        string position;
} interval;
class Config {
    
public:
    //Config(char* configFile);
    Config(string configFile);
    void init();
    string filename;
    string outputDir;
    int step;
    bool grid;
    float alpha,samplingFreq,clockFreq;
    vector<interval> intervals;
    
private:
    string configFile;
    ifstream config;
};
#endif
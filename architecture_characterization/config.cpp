#include "config.hpp"
/*Config::Config(char* fn) {
    configFile=string(fn);
}*/
Config::Config(string fn) {
    configFile=fn;
}
void Config::init() {
    config.open(configFile,ifstream::in);
    if (!config.is_open()) {
        cout<<"Can't open config file."<<endl;
        exit(0);
    }
    //parse config file and put params in public class attributes
    try
    {
        ptree pt;
        info_parser::read_info(config, pt);
        try
        {
            //parse the first level
            filename = pt.get<string>("filename");
            outputDir= pt.get<string>("outputDir");
            step= pt.get<int>("step");
            alpha= pt.get<float>("alpha");
            samplingFreq=pt.get<float>("samplingFreq");
            clockFreq=pt.get<float>("clockFreq");
            grid=pt.get<bool>("displayGrid");
            if(samplingFreq<0 || clockFreq<0) {
                cout<<"Invalid frequency."<<endl;
                exit(0);
            }
            //parse the intervals and put all inside the vector
            ptree windows (pt.get_child("intervals"));
            ptree::const_iterator intIt;
            for(intIt=windows.begin();intIt!=windows.end();++intIt) {
                interval i;
                i.name=intIt->first;
                i.start=intIt->second.get<int>("start");
                i.end=intIt->second.get<int>("end");
                if(i.start<0 || i.end<0 || i.start>i.end) {
                    cout<<"Wrong start end interval."<<endl;
                    exit(0);
                }
                string k=intIt->second.get<string>("key");
                i.key=strtol(k.c_str(),NULL,16);  
                i.model=intIt->second.get<string>("model");
                int bw=intIt->second.get<int>("bw");
                i.bw=(bw==1 ? true : false);
                ptree modelParams (intIt->second.get_child("modelParams"));
                i.keySize=modelParams.get<int>("keySize");
                i.sbox=modelParams.get<int>("sbox");
                i.position=modelParams.get<string>("position");
                i.intSize=modelParams.get<int>("intSize");
                intervals.push_back(i);
            }            
        } catch( ptree_error e) {
            cerr << "Analysis configuration error. Check the config file" << endl;
            exit (3);
        }
    } catch ( info_parser::info_parser_error e) {
        cerr << "Cannot parse Analysis configuration" << endl;
        exit ( 3 );
    }
    
}

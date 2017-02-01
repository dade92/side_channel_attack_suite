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
    try {
        ptree pt;
        info_parser::read_info(config, pt);
        try {
            filename=pt.get<string>("filename");
            outputFilename=pt.get<string>("outputFilename");
            step=pt.get<int>("step");
            samplingFreq=pt.get<float>("samplingFreq");
            string padString=pt.get<string>("padding");
            if(padString.compare("zero")==0)
                pad=zero;
            else if(padString.compare("mean")==0)
                pad=mean;
            else if(padString.compare("hold")==0)
                pad=hold;
            if(samplingFreq<0) {
                cout<<"Wrong sampling frequency."<<endl;
                exit(0);
            }
            ptree windowsPtree (pt.get_child("windows"));
            ptree::const_iterator windowIt;
            string windowTypeString,filterTypeString;
            for(windowIt=windowsPtree.begin();windowIt!=windowsPtree.end();++windowIt) {
                window w;
                w.name=windowIt->first;
                filterTypeString=windowIt->second.get<string>("filterType");
                if(filterTypeString.compare("lowPass")==0)
                    w.type=lowPass;
                else if(filterTypeString.compare("bandPass")==0)
                    w.type=bandPass;
                else if(filterTypeString.compare("highPass")==0)
                    w.type=highPass;
                else {
                    cout<<"Wrong band type."<<endl;
                    exit(0);
                }
                windowTypeString=windowIt->second.get<string>("windowType");
                if(windowTypeString.compare("rect")==0)
                    w.windowFunction=rect;
                else {
                    cout<<"Wrong window filter type."<<endl;
                    exit(0);
                }
                if(w.type==lowPass)
                    w.highFrequency=windowIt->second.get<float>("highFrequency");
                else if(w.type==highPass)
                    w.lowFrequency=windowIt->second.get<float>("lowFrequency");
                else {
                    w.lowFrequency=windowIt->second.get<float>("lowFrequency");
                    w.highFrequency=windowIt->second.get<float>("highFrequency");
                }
                windows.push_back(w);
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
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
            filterFile="";
            filename=pt.get<string>("filename");
            outputFilename=pt.get<string>("outputFilename");
            step=pt.get<int>("step");
            samplingFreq=pt.get<float>("samplingFreq");
            if(samplingFreq<0) {
                cout<<"Wrong sampling frequency."<<endl;
                exit(0);
            }
            string padString=pt.get<string>("padding");
            if(padString.compare("zero")==0)
                pad=zero;
            else if(padString.compare("mean")==0)
                pad=mean;
            else if(padString.compare("hold")==0)
                pad=hold;
            string filterCombString=pt.get<string>("filterCombination");
            if(filterCombString.compare("doNothing")==0)
                filterComb=doNothing;
            else if(filterCombString.compare("clamp")==0)
                filterComb=clamp;
            else if(filterCombString.compare("normalize")==0)
                filterComb=normalize;
            else {
                cout<<"Wrong filter combination options."<<endl;
                exit(0);
            }
            try {
                filterFile=pt.get<string>("filterFile");
            } catch( ptree_error e) {}
            ptree windowsPtree (pt.get_child("windows"));
            ptree::const_iterator windowIt;
            string windowTypeString,filterTypeString;
            //parses the list of windows
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
                else if(windowTypeString.compare("hann")==0)
                    w.windowFunction=hann;
                else if(windowTypeString.compare("nuttall")==0)
                    w.windowFunction=nuttall;
                else {
                    cout<<"Wrong window filter type."<<endl;
                    exit(0);
                }
                if(w.type==lowPass) {
                    w.highFrequency=windowIt->second.get<float>("highFrequency");
                    w.lowFrequency=0;
                }
                else if(w.type==highPass) {
                    w.lowFrequency=windowIt->second.get<float>("lowFrequency");
                    w.highFrequency=samplingFreq/2;
                } else {
                    w.lowFrequency=windowIt->second.get<float>("lowFrequency");
                    w.highFrequency=windowIt->second.get<float>("highFrequency");
                }
                if(w.lowFrequency>w.highFrequency || w.lowFrequency>samplingFreq/2
                    || w.highFrequency>samplingFreq/2) {
                    cout<<"Wrong frequencies."<<endl;
                    exit(0);
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
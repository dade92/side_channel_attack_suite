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
            maxTau=pt.get<int>("maxTau");
            if(maxTau<0) {
                cout<<"maxTau should be a positive integer."<<endl;
                exit(0);
            }
            string typeString=pt.get<string>("alignmentFunction");
            if(typeString.compare("crossCorr")==0)
                function=crossCorr;
            else {
                cout<<"Unrecognized alignment function."<<endl;
                exit(0);
            }
            startSample=pt.get<int>("startSample");
            endSample=pt.get<int>("endSample");
            if(startSample>endSample || startSample<0 || endSample<0) {
                cout<<"Wrong start/end sample."<<endl;
                exit(0);
            }
            string modeString=pt.get<string>("mode");
            if(modeString.compare("multiple")==0)
                m=multiple;
            else if(modeString.compare("single")==0) {
                m=single;
                ptree singleModePtree (pt.get_child("singleMode"));
                samplingFreq=singleModePtree.get<float>("samplingFreq");
                if(samplingFreq<0) {
                    cout<<"Invalid sampling frequency."<<endl;
                    exit(0);
                }
                cipherTime=singleModePtree.get<float>("cipherTime");
                if(cipherTime<0) {
                    cout<<"Invalid cipher time."<<endl;
                    exit(0);
                }
                if(endSample-startSample<cipherTime*samplingFreq) {
                    cout<<"Invalid start end sample and cipher time"<<endl;
                    exit(0);
                }
                startPlain=singleModePtree.get<string>("startPlain");
                if(startPlain.size()%2!=0) {
                    cout<<"Invalid start plain."<<endl;
                    exit(0);
                }
                key=singleModePtree.get<string>("key");
                if(key.size()!=startPlain.size()) {
                    cout<<"Invalid key or plain size."<<endl;
                    exit(0);
                }
                printCorrelation=singleModePtree.get<bool>("printCorr");
            }
            else {
                cout<<"Mode not recognized."<<endl;
                exit(0);
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
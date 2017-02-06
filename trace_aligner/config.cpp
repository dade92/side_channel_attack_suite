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
        } catch( ptree_error e) {
            cerr << "Analysis configuration error. Check the config file" << endl;
            exit (3);
        }        
    } catch ( info_parser::info_parser_error e) {
        cerr << "Cannot parse Analysis configuration" << endl;
        exit ( 3 );
    }
}
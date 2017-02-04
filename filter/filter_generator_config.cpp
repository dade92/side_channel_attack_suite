#include "filter_generator_config.hpp"
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
            string padString=pt.get<string>("padding");
            if(padString.compare("zero")==0)
                pad=zero;
            else if(padString.compare("mean")==0)
                pad=mean;
            else if(padString.compare("hold")==0)
                pad=hold;
        } catch( ptree_error e) {
            cerr << "Analysis configuration error. Check the config file" << endl;
            exit (3);
        }        
    } catch ( info_parser::info_parser_error e) {
        cerr << "Cannot parse Analysis configuration" << endl;
        exit ( 3 );
    }
}
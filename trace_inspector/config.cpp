#include "config.hpp"

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
            batch= pt.get<int>("batch");
            if(batch<0) {
                cout<<"Invalid batch"<<endl;
                exit(0);
            }
            samplingFreq=pt.get<float>("samplingFreq");
            clockFreq=pt.get<float>("clockFreq");
            if(samplingFreq<0 || clockFreq<0) {
                cout<<"Invalid frequency."<<endl;
                exit(0);
            }
            startSample=pt.get<int>("startSample");
            maxSample=pt.get<int>("endSample");
            grid=pt.get<bool>("displayGrid");
            xtics=pt.get<int>("xtics");
            if(startSample<0 || startSample>maxSample) {
                cout<<"Invalid start/ending sample"<<endl;
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

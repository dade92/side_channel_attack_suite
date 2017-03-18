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
            modeString=pt.get<string>("mode");
            if(modeString.compare("char")==0)
                m=charac;
            else if(modeString.compare("aes")==0)
                m=aes;
            else if(modeString.compare("ttable")==0)
                m=ttable;
            else {
                cout<<"Mode not recognized."<<endl;
                exit(0);
            }
            ptree imageParams(pt.get_child("imageParams"));
            grid=imageParams.get<bool>("displayGrid");
            xtics=imageParams.get<float>("xtics");
            bw=imageParams.get<bool>("bw");
            unitString=imageParams.get<string>("unit");
            figureWidth=imageParams.get<float>("figureWidth");
            figureHeight=imageParams.get<float>("figureHeight");
            scale=imageParams.get<float>("scale");
            if(scale<0) {
                cout<<"Invalid scale."<<endl;
                exit(0);
            }
            if(figureWidth<=0 || figureHeight<=0) {
                cout<<"Invalid output size"<<endl;
                exit(0);
            }
            if(unitString.compare("samples")==0)
                unit=samples;
            else if(unitString.compare("seconds")==0)
                unit=seconds;
            else {
                cout<<"Invalid unit."<<endl;
                exit(0);
            }
            if(samplingFreq<0 || clockFreq<0) {
                cout<<"Invalid frequency."<<endl;
                exit(0);
            }
            timescaleString=imageParams.get<string>("timescale");
            if(timescaleString.compare("absolute")==0)
                timescale=absolute;
            else if(timescaleString.compare("relative")==0)
                timescale=relative;
            else {
                cout<<"Invalid timescale."<<endl;
                exit(0);
            }
            abs_value=imageParams.get<bool>("abs");
            latexOutput=imageParams.get<bool>("latexOutput");
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
                ptree modelParams (intIt->second.get_child("modelParams"));
                i.keySize=modelParams.get<int>("keySize");
                i.sbox=modelParams.get<int>("sbox");
                i.position=modelParams.get<string>("position");
                i.intSize=modelParams.get<int>("intSize");
                intervals.push_back(i);
            }            
        } catch( ptree_error e) {
            cerr << "Analysis configuration error. Maybe some parameter is missing."<<endl
            <<"Check the config file." << endl;
            exit (3);
        }
    } catch ( info_parser::info_parser_error e) {
        cerr << "Can't parse correctly, check the config file." << endl;
        exit ( 3 );
    }
    
}

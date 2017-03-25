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
            ptree files(pt.get_child("files"));
            //at least one must be present, otherwise exit
            filenames.push_back(files.get<string>("filename1"));
            try {
                int i=2;
                while(1) {
                    string s="filename"+std::to_string(i);
                    filenames.push_back(files.get<string>(s));
                    i++;
                }
            }catch(ptree_error e) {}
            outputFilename=pt.get<string>("outputFilename");
            step=pt.get<int>("step");
            string typeString=pt.get<string>("alignmentFunction");
            if(typeString.compare("crossCorr")==0)
                function=crossCorr;
            else {
                cout<<"Unrecognized alignment function."<<endl;
                exit(0);
            }
            maxTau=pt.get<int>("maxTau");
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
                /*if(endSample-startSample<cipherTime*samplingFreq) {
                    cout<<"Invalid start end sample and cipher time"<<endl;
                    exit(0);
                }*/
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
                ptree originalFiles(singleModePtree.get_child("originalFiles"));
                //at least one file
                originalFilenames.push_back(originalFiles.get<string>("filename1"));
                try {
                    int i=2;
                    while(1) {
                        string s="filename"+std::to_string(i);
                        originalFilenames.push_back(originalFiles.get<string>(s));
                        i++;
                    }
                }catch(ptree_error e) {}
            }
            else {
                cout<<"Mode not recognized."<<endl;
                exit(0);
            }
        } catch( ptree_error e) {
            cerr << "Analysis configuration error. Check the config file." << endl;
            exit (3);
        }        
    } catch ( info_parser::info_parser_error e) {
        cerr << "Cannot parse config file." << endl;
        exit ( 3 );
    }
}
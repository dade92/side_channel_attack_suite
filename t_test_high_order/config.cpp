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
            filename2= pt.get<string>("filename2");
            batch=pt.get<int>("batch");
            if(batch<0) {
                cout<<"Invalid batch."<<endl;
                exit(0);
            }
            alpha=pt.get<float>("alpha");
            if(alpha<0 || alpha>1) {
                cout<<"Invalid alpha."<<endl;
                exit(0);
            }
            samplingFreq=pt.get<float>("samplingFreq");
            clockFreq=pt.get<float>("clockFreq");
            if(samplingFreq<0 || clockFreq<0) {
                cout<<"Invalid frequency."<<endl;
                exit(0);
            }
            grid=pt.get<bool>("displayGrid");            
            if(batch<0) {
                cout<<"Invalid batch size."<<endl;
                exit(0);
            }
            startSample=pt.get<int>("startSample");
            maxSample=pt.get<int>("endSample");
            if(startSample<0 || startSample>maxSample) {
                cout<<"Invalid start/ending sample."<<endl;
                exit(0);
            }
            order=pt.get<int>("order");
            if(order<1 || order>5) {
                cout<<"Invalid test order."<<endl;
                exit(0);
            } else {
                //TODO:put another ptree for the order params
                if(order==2) {
                    maxTau2=pt.get<int>("maxTau2");
                    if(maxTau2<0) {
                        cout<<"Invalid tau."<<endl;
                        exit(0);
                    }
                }
                else if(order==3) {
                    maxTau2=pt.get<int>("maxTau2");
                    maxTau3=pt.get<int>("maxTau3");
                    if(maxTau2<0 || maxTau3<0 || maxTau2<maxTau3) {
                        cout<<"Invalid tau."<<endl;
                        exit(0);
                    }
                } else if(order==4) {
                    maxTau2=pt.get<int>("maxTau2");
                    maxTau3=pt.get<int>("maxTau3");
                    maxTau4=pt.get<int>("maxTau4");
                    if(maxTau2<=0 || maxTau3<=0 || maxTau4<=0 || 
                        maxTau2<=maxTau3 || maxTau3<=maxTau4) {
			cout<<"invalid tau."<<endl;
			exit(1);
                    }
                } else if(order==5) {
                    maxTau2=pt.get<int>("maxTau2");
                    maxTau3=pt.get<int>("maxTau3");
                    maxTau4=pt.get<int>("maxTau4");
                    maxTau5=pt.get<int>("maxTau5");
                    if(maxTau2<=0 || maxTau3<=0 || maxTau4<=0 || 
                        maxTau5<=0 || maxTau2<=maxTau3 || 
                        maxTau3<=maxTau4 || maxTau4<=maxTau5) {
			cout<<"invalid tau."<<endl;
			exit(1);
                    }
                }
            }
        } catch( ptree_error e) {
            cerr << "Analysis configuration error. Check the config file." << endl;
            exit (3);
        }
    } catch ( info_parser::info_parser_error e) {
        cerr << "Cannot parse Analysis configuration." << endl;
        exit ( 3 );
    }
    
}

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
            outputDir= pt.get<string>("outputDir");
            batch= pt.get<int>("step");
            if(batch<0) {
                cout<<"Invalid batch"<<endl;
                exit(0);
            }
            maxTrace=pt.get<int>("maxTraces");
            if(maxTrace<0) {
                cout<<"Invalid max trace."<<endl;
                exit(0);
            }
            samplingFreq=pt.get<float>("samplingFreq");
            clockFreq=pt.get<float>("clockFreq");
            tracesToPrint=pt.get<int>("numTraces");
            if(tracesToPrint<0 || tracesToPrint>batch) {
                cout<<"Invalid trace to print or trace to print greter than step size"<<endl;
                exit(0);
            }
            if(samplingFreq<0 || clockFreq<0) {
                cout<<"Invalid frequency."<<endl;
                exit(0);
            }
            startSample=pt.get<int>("startSample");
            maxSample=pt.get<int>("endSample");
            startBin=pt.get<int>("startFrequency");
            endBin=pt.get<int>("endFrequency");
            if(startBin<-samplingFreq/2 || endBin>samplingFreq/2 || startBin>endBin) {
                cout<<"Wrong start/end frequency."<<endl;
                exit(0);
            }
            startBin=(startBin!=0 ? startBin : -samplingFreq/2);
            endBin=(endBin!=0 ? endBin : samplingFreq/2);
            ptree imageParams(pt.get_child("imageParams"));
            grid=imageParams.get<bool>("displayGrid");
            xtics=imageParams.get<int>("xtics");
            unitString=imageParams.get<string>("unit");
            if(unitString.compare("samples")==0)
                unit=samples;
            else if(unitString.compare("seconds")==0)
                unit=seconds;
            else {
                cout<<"Invalid unit."<<endl;
                exit(0);
            }
            figureWidth=imageParams.get<float>("figureWidth");
            figureHeight=imageParams.get<float>("figureHeight");
            if(figureWidth<=0 || figureHeight<=0) {
                cout<<"Invalid output size"<<endl;
                exit(0);
            }
            if(startSample<0 || startSample>maxSample && maxSample!=0) {
                cout<<"Invalid start/ending sample"<<endl;
                exit(0);
            }
            logScale=imageParams.get<bool>("logScale");
            latexOutput=imageParams.get<bool>("latexOutput");
        } catch( ptree_error e) {
            cerr << "Analysis configuration error. Check the config file" << endl;
            exit (3);
        }
    } catch ( info_parser::info_parser_error e) {
        cerr << "Cannot parse Analysis configuration" << endl;
        exit ( 3 );
    }
    
}

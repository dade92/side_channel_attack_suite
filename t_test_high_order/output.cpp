#include "output.hpp"
Output::Output(Config& c,Input& input1,Input& input2) {
    order=c.order;
    samplesPerTrace=input1.samplesPerTrace;
    startSample=c.startSample;
    endSample=c.maxSample;
    grid=c.grid;
}

void Output::writeResults(std::vector<float *>& tStatistics,std::vector<float *>& pStatistics) {
    string datName="tvalue";
    string scriptName="tvalue";

    std::ofstream outputScript,outputDat,outputPScript,outputPDat;
    
    outputScript.open(scriptName+".gpl");
    if(outputScript.fail()) {
        cerr << "Please provide a correct output script filename" << endl;
        exit(0);
    }
    outputDat.open(datName+".dat");
    if(outputDat.fail()) {
        cerr << "Please provide a correct output dat filename" << endl;
        exit(0);
    }
    //for p value data, I give standard names
    outputPDat.open("pvalue.dat");
    if(outputScript.fail()) {
        cerr << "Please provide a correct output script filename" << endl;
        exit(0);
    }
    outputPScript.open("pvalue.gpl");
    if(outputPDat.fail()) {
        cerr << "Please provide a correct output script filename" << endl;
        exit(0);
    }
    for(int i=startSample;i<endSample;i++) {
	outputDat<<i<<" ";
	outputPDat<<i<<" ";
	for(unsigned int n=0;n<tStatistics.size();n++) {
            outputDat<<tStatistics[n][i]<<" ";
            outputPDat<<pStatistics[n][i]<<" ";
        }
	outputDat<<endl;
	outputPDat<<endl;
    }
    outputScript << "set term png size 2000,1280;" << endl;
    outputScript << "set output \""<< "tvalue" << "_order"<<order<<".png\";" << endl;
    outputScript << "set autoscale;" << endl;
    outputScript << "set xtic auto;" << endl;
    outputScript << "set ytic auto;" << endl;
    outputScript << "set xrange [" << startSample << ":" << endSample<< "];" << endl;
    outputScript << "unset key;" << endl;
    outputScript << "set xlabel \"Sample\";" << endl;
    outputScript << "set ylabel \"t\";" << endl << endl;

    outputPScript << "set term png size 2000,1280;" << endl;
    outputPScript << "set output \""<< "pvalue" << "_order"<<order<<".png\";" << endl;
    outputPScript << "set autoscale;" << endl;
    outputPScript << "set xtic auto;" << endl;
    outputPScript << "set ytic auto;" << endl;
    outputPScript << "set xrange [" << startSample << ":" << endSample << "];" << endl;
    outputPScript << "unset key;" << endl;
    outputPScript << "set xlabel \"Sample\";" << endl;
    outputPScript << "set ylabel \"t\";" << endl << endl;

    outputScript <<"plot ";
    outputPScript<<"plot ";
    for(unsigned int iSample=0;iSample<tStatistics.size();iSample++) {
    	if(iSample!=0) {
    		outputScript<<",";
    		outputPScript<<",";
    	}
    	outputScript<<"\""<<datName+".dat"<<"\" ";
    	outputPScript<<"\""<<"pvalue.dat"<<"\" ";
    	outputScript << "u 1:" << iSample+2 << " ";
    	outputPScript << "u 1:" << iSample+2 << " ";
    	outputScript << "with lines linecolor \"black\"";
    	outputPScript <<"with lines linecolor \"black\"";
    }
    outputScript <<	";"<<endl;
    cout<<"Data saved. You can find gnuplot script in file \""<<scriptName<<".gpl\" and data in file \""<<datName<<".dat\"."<<endl
    <<"p values in file \"pvalue.gpl\" and data in \"pvalue.dat\""<<endl;
}

string Output::getKeyAsString(int key) {
    std::stringstream s;
    s << std::hex;
    s << key;
    return s.str();
}
#include "output.hpp"
Output::Output(Config& c,Input& input1,Input& input2) {
    order=c.order;
    samplesPerTrace=input1.samplesPerTrace;
}

void Output::writeResults(std::vector<float *>& tStatistics,std::vector<float *>& pStatistics) {
    string datName="tTest.dat";
    string scriptName="tTest.gpl";

    std::ofstream outputScript,outputDat,outputPScript,outputPDat;
    //initializes the ostream used to prepare gnuplot scripts
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
    outputPDat.open("output_ttest_pvalue.dat");
    if(outputScript.fail()) {
        cerr << "Please provide a correct output script filename" << endl;
        exit(0);
    }
    outputPScript.open("output_ttest_pvalue.gpl");
    if(outputPDat.fail()) {
        cerr << "Please provide a correct output script filename" << endl;
        exit(0);
    }
    for(int i=0;i<samplesPerTrace;i++) {
	outputDat<<i<<" ";
	outputPDat<<i<<" ";
	for(unsigned int n=0;n<tStatistics.size();n++) {
            outputDat<<tStatistics[n][i]<<" ";
            outputPDat<<pStatistics[n][i]<<" ";
        }
	outputDat<<endl;
	outputPDat<<endl;
    }
    outputScript << "set term svg dynamic size 8000,1280;" << endl;
    outputScript << "set output \""<< scriptName << "_order="<<order<<".svg\";" << endl;
    outputScript << "set autoscale;" << endl;
    outputScript << "set xtic auto;" << endl;
    outputScript << "set ytic auto;" << endl;
    outputScript << "set xrange [" << 0 << ":" << samplesPerTrace << "];" << endl;
    outputScript << "unset key;" << endl;
    outputScript << "set xlabel \"Sample\";" << endl;
    outputScript << "set ylabel \"t\";" << endl << endl;

    outputPScript << "set term svg dynamic size 8000,1280;" << endl;
    outputPScript << "set output \""<< "output_ttest_tvalue" << "_order="<<order<<".svg\";" << endl;
    outputPScript << "set autoscale;" << endl;
    outputPScript << "set xtic auto;" << endl;
    outputPScript << "set ytic auto;" << endl;
    outputPScript << "set xrange [" << 0 << ":" << samplesPerTrace << "];" << endl;
    outputPScript << "unset key;" << endl;
    outputPScript << "set xlabel \"Sample\";" << endl;
    outputPScript << "set ylabel \"t\";" << endl << endl;

    outputScript << "plot ";
    outputPScript<< "plot ";
    for(unsigned int iSample=0;iSample<tStatistics.size();iSample++) {
    	if(iSample!=0) {
    		outputScript<<",";
    		outputPScript<<",";
    	}
    	outputScript<<"\""<<datName+".dat"<<"\" ";
    	outputPScript<<"\""<<"output_ttest_pvalue.dat"<<"\" ";
    	outputScript << "u 1:" << iSample+2 << " ";
    	outputPScript << "u 1:" << iSample+2 << " ";
    	outputScript << "t \"" << iSample+1 << "\" ";
    	outputPScript << "t \"" << iSample+1 << "\" ";
    	outputScript << "with lines";
    	outputPScript <<"with lines";
    }
    outputScript <<	";"<<endl;
    outputPScript<<";"<<endl;
    outputScript << "set term png size 8000,1280;" << endl;
    outputScript << "set output \""<< scriptName << "_order="<<order<<".png\";" << endl;
    outputScript << "set autoscale;" << endl;
    outputScript << "set xtic auto;" << endl;
    outputScript << "set ytic auto;" << endl;
    outputScript << "set xrange [" << 0 << ":" << samplesPerTrace << "];" << endl;
    outputScript << "unset key;" << endl;
    outputScript << "set xlabel \"Sample\";" << endl;
    outputScript << "set ylabel \"t\";" << endl << endl;

    outputScript << "plot ";
    for(unsigned int iSample=0;iSample<tStatistics.size();iSample++) {
    	if(iSample!=0) outputScript<<",";
    	outputScript<<"\""<<datName+".dat"<<"\" ";
    	outputScript << "u 1:" << iSample+2 << " ";
    	outputScript << "t \"" << iSample+1 << "\" ";
    	outputScript << "with lines";
    }
    outputScript <<	";"<<endl;
    cout<<"Data saved. You can find gnuplot script in file "<<scriptName<<".gpl and data in file "<<datName<<".dat; p values in output_ttest_pvalue.gpl"<<endl;
}

string Output::getKeyAsString(int key) {
    std::stringstream s;
    s << std::hex;
    s << key;
    return s.str();
}
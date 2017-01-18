#include "output.hpp"
Output::Output(Config& c,Input& input) {
    outputDir=c.outputDir;
    numIntervals=c.intervals.size();
    step=c.step;
    numSteps=input.numTraces/c.step;
    alpha=c.alpha;
    intervals=vector<interval>(c.intervals);
    filename=c.filename;
    numTraces=input.numTraces;
    samplingFreq=c.samplingFreq;
    clockFreq=c.clockFreq;
    grid=c.grid;
    bw=c.bw;
    xtics=c.xtics;
    unit=c.unit;
    figureWidth=c.figureWidth;
    figureHeight=c.figureHeight;
}

void Output::writeResults(vector<result*>& results,vector<float**>& finalPearson) {
    ofstream logStream(outputDir+"/"+"logFile",ofstream::out);
    //write some useful information about the .dat file
    logStream<<"Log of the file: "<<filename<<":"<<endl;
    logStream<<"numTraces: "<<numTraces<<endl<<endl;
    logStream<<boost::format("%-32s %-25s %-25s %-12s %-15s %-10s %-7s %-8s %-10s\n") % 
    "interval" % "bestKeyFound!=correct" % "bestPearson!=correct" % "correctKey" % 
    "PearsonCorrect" % "relevant?" % "found?" % "model" % "position";
    logStream<<endl;
    //for each interval
    for(int i=0;i<numIntervals;i++) {
        int keySpace=pow(2,intervals[i].keySize);
        //sets the name of the files
        string confidenceScriptName("confidenceScript"+intervals[i].name+".gpl");
        string confidenceDatName("confidenceDat"+intervals[i].name+".dat");
        string scriptName("script"+intervals[i].name+".gpl");
        string datName("dat"+intervals[i].name+".dat");
        //opens the streams
        ofstream confidenceScriptStream(outputDir+"/"+confidenceScriptName,ifstream::out);
        ofstream confidenceDatStream(outputDir+"/"+confidenceDatName,ifstream::out);
        ofstream scriptStream(outputDir+"/"+scriptName,ifstream::out);
        ofstream datStream(outputDir+"/"+datName,ifstream::out);
        if(!confidenceScriptStream.is_open() || !confidenceDatStream.is_open() 
            || !scriptStream.is_open() || !datStream.is_open()) {
            cout<<"Can't opening output files."<<endl;
            exit(0);
        }
        //writes the confidence.dat file, for each interval
        //take the array of results of this specific interval
        result* r=results[i];
        int count=0;
        while(count!=numSteps) {
            confidenceDatStream<<(count+1)*step<<" ";
            confidenceDatStream<<r[count].pearson<<" "<<r[count].pearson-r[count].ic/2<<" "
            <<r[count].pearson+r[count].ic/2<<" "
            <<r[count].pearsonCorrect<<" "<<r[count].pearsonCorrect-r[count].ic/2<<" "
            <<r[count].pearsonCorrect+r[count].ic/2<<endl;
            count++;
        }    
        //write the gnuplot confidence script, for each interval
        confidenceScriptStream<<"set term png size "<<figureWidth<<", "<<figureHeight<<endl;
        confidenceScriptStream<<"set output \"confidence"<<intervals[i].name<<".png\";"<<endl;
        confidenceScriptStream<<"set autoscale;"<<endl;
        confidenceScriptStream<<"unset key"<<endl;
        confidenceScriptStream << "set xtics font \"arial,20\" " <<endl;
        confidenceScriptStream << "set ytics font \"arial,20\" " <<endl;
        confidenceScriptStream<<"set title\""<<intervals[i].name<<" confidence\";"<<endl;
        confidenceScriptStream<<"set xlabel\"number of traces\" font \"arial,20\";"<<endl;
        confidenceScriptStream<<"set ylabel\"max Pearson coefficienct with alpha="<<alpha
            <<"\" font \"arial,20\";"<<endl<<endl<<endl;
        confidenceScriptStream<<"plot";
        //six lines to plot
        for(int k=0;k<6;k++) {
            confidenceScriptStream  << " \"" << confidenceDatName << "\" u 1:" 
            << k + 2 << " t \"" << this->getKeyAsString ( k ) << "\" with lines ";
            //print in grey the wrong key, in black the correct key
            if(k<3) confidenceScriptStream<<"linecolor \"grey\"";
            else confidenceScriptStream<<"linecolor \"black\"";
            //the lines of the confidence intervals are dashed
            if(k==1 || k==2 || k==4 || k==5) confidenceScriptStream<<" linetype \"dashed\"";
            //if I'm not at the end of the script, make a comma
            if(k!=5) confidenceScriptStream<<",";
        }        
        
        
        //write the gnuplot script file
        scriptStream<<endl<<"set term png size "<<figureWidth<<", "<<figureHeight<<endl;
        scriptStream<<"set output \""<<intervals[i].name<<".png\";"<<endl;
        scriptStream<<"set autoscale;"<<endl;
        scriptStream<<"set lmargin 10;set rmargin 10;"<<endl;
        if(grid) {
            if(xtics==0) {
                switch(unit) {
                    case samples:
                        scriptStream << "set xtics "<<samplingFreq/clockFreq<<" font \",25\" " <<endl;
                        break;
                    case seconds:
                        scriptStream << "set xtics "<<1/clockFreq<<" font \",25\" " <<endl;
                        break;
                }
                
            }
            else
                scriptStream << "set xtics "<<xtics<<" font \",25\" " <<endl;
            scriptStream<<"set grid xtics "<<" lt 0 lc  rgb \"grey\" lw 1;"<<endl;
            scriptStream<<"set grid ytics "<<" lt 0 lc  rgb \"grey\" lw 1;"<<endl;
        }
        else {
            if(xtics==0)
                scriptStream << "set xtics auto font \"arial,25\" " <<endl;
            else
                scriptStream << "set xtics "<<xtics<<" font \"arial,25\" " <<endl;
        }
        scriptStream << "set ytics font \"arial,25\" " <<endl;
        scriptStream<<"set title\""<<intervals[i].name<<"\";"<<endl;
        switch(unit) {
            case samples:
                scriptStream<<"set xlabel\"Time\" font \"arial,20\";"<<endl;
                break;
            case seconds:
                scriptStream<<"set xlabel\"Time[us]\" font \"arial,20\";"<<endl;
                break;
        }
        scriptStream<<"set ylabel\"Pearson coefficient\" font \"arial,20\";"<<endl;
        if(bw) scriptStream<<"unset key"<<endl;
        else scriptStream << "set key outside right;" << endl;
        scriptStream<<"plot";
        //keyspace lines to plot. If there is only one key, prints it in black color
        if(intervals[i].keySize==0) {
            scriptStream  << " \"" << datName << "\" u 1:2" << " t \"" << this->getKeyAsString ( 0 ) << "\" with lines";
            scriptStream <<" linecolor \"black\"";
        }
        else {
            for ( int k = 0; k < keySpace; k++ ) {
		scriptStream  << " \"" << datName << "\" u 1:" << k + 2 << " t \"" << this->getKeyAsString ( k ) << "\" with lines";
                if(bw) {
                    if(k==intervals[i].key) scriptStream<<" linecolor \"black\"";
                    else scriptStream<<" linecolor \"grey\"";
                }
		if ( k != keySpace - 1 ) {
			scriptStream  << ",";
		}
            }
        }
        //writes the .dat file
        //for each column
        for(int t=0;t<intervals[i].end-intervals[i].start;t++) {
            switch(unit) {
                case samples:
                    datStream<<t<<" ";
                    break;
                case seconds:
                    datStream<<(t)/samplingFreq<<" ";
                    break;
            }
            //for each row
            for(int k=0;k<keySpace;k++) {
                /*writes 
                 * 0 k1 k2 ..... kn
                 * 1 k1 k2 ..... kn
                 * ...
                 */
                datStream<<finalPearson[i][k][t]<<" ";
            }
            datStream<<endl;
        }    
        //conditions for statistical relevance:
        //if i'm wrong for sure, if i'm right for sure, if the best keys are the same
        bool cond1=results[i][numSteps-1].pearson-results[i][numSteps-1].ic/2 >
            results[i][numSteps-1].pearsonCorrect+results[i][numSteps-1].icCorrect/2;
            
        bool cond2=results[i][numSteps-1].pearson+results[i][numSteps-1].ic/2 <
            results[i][numSteps-1].pearsonCorrect-results[i][numSteps-1].icCorrect/2;
        
        //write the log line for this interval
        // interval name,best key found,pearson of the best,correct key,Pearson correct,statistically relevant
        string name=intervals[i].name+"["+to_string(intervals[i].start)+":"+to_string(intervals[i].end)+"]:";
        logStream<<boost::format("%-35s %-25s %-25.5f %-10s %-15.5f %-10s %-8s %-8s %-10s\n") % name %
        getKeyAsString(results[i][numSteps-1].bestKey) % results[i][numSteps-1].pearson %
        getKeyAsString(intervals[i].key) % results[i][numSteps-1].pearsonCorrect %
        (cond1 || cond2 ? "YES" : "NO") % 
        (results[i][numSteps-1].pearsonCorrect>results[i][numSteps-1].pearson ? "YES" : "NO") %
        intervals[i].model % intervals[i].position;
        //verbose..
        cout<<"You can find in the directory \""<<outputDir<<"\" the gnuplot scripts respectively with names:\n"
            <<"\""<<confidenceScriptName<<"\" and \""<<scriptName<<"\".\nEvery script produces a .png file with the"
            <<" name of the associated interval"<<endl<<endl;
        //closes the streams
        confidenceScriptStream.close();
        confidenceDatStream.close();
        scriptStream.close();
        datStream.close();
    }
}

string Output::getKeyAsString(int key) {
    std::stringstream s;
    s << std::hex;
    s << key;
    return s.str();
}
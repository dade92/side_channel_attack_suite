    Trace aligner of side channel suite.
    It takes as input a config file, an example
    in config_example,with the name of the input 
    .dat file, and  other params such as the 
    max delay to consider, and outputs:
    
    In multiple mode, a .dat file with 
    the aligned traces (it takes as 
    reference trace the first one, and
    aligns the others based on that trace).
    
    In single mode, the aligner splits a 
    single long trace into different ones,
    computing the auto correlation of the trace.
    The auto correlation is plotted, this is
    useful to see patterns in the traces.
    Consider that the tool takes as input a list
    of files to analyze, not only one single file:
    in this way the analysis is still possible with different
    single "long" traces.
    
    Usage:
    ./trace_aligner.out configFile

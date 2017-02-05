    This is the filter tool of the Side Channel suite.
    It takes as input a config file, in which is indicated
    the file of the traces to filter, and other parameters.
    It produces another .dat file containing the filtered
    traces. The implemented filters are:
    
    -rect,
    -Hann,
    -Nuttall 
    
    Adaptive filter generator instead is a tool that generates
    an adaptive filter taking as input a set of dpacalc traces,
    producing as output a .dat file that contains the filter.
    In order to pass the generated filter to the filter program,
    un-comment in config file the param "filterFile".
    
    IMPORTANT: the fftw3 library that computes the DFT and IDFT
    was compiled passing --enable-float parameter to the 
    configure script. In this way the library supports 
    computation with floating numbers. If you don't configure 
    in that way, the compiler will NOT resolve the function 
    calls correctly. If you want to use the double estension instead,
    then find in the code the string fftwf and replace with fftw.
    
    Usage:
    ./filter.out config_file
    ./adaptive_filter_generator.out config_file

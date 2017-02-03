    This is the filter tool of the Side Channel suite.
    It takes as input a config file, in which is indicated
    the file of the traces to filter, and other parameters.
    It produces another .dat file containing the filtered
    traces. The implemented filters are:
    
    -rect, a rectangular filter
    
    IMPORTANT: the fftw3 library that computes the DFT and IDFT
    was compiled passing --enable-float to the configure script.
    In this way the library supports computation with floating
    numbers. If you don't configure in that way, the compiler
    will NOT resolve the function calls correctly. If you want
    to use the double estension, then find in the code the 
    string fftwf and replace with fftw.
    
    Usage:
    ./filter.out config_file

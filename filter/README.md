    Filter tool of the Side Channel suite.
    It takes as input a config file, an example in config_example,
    in which is indicated the file of the traces to filter, 
    and other parameters.
    It produces another .dat file containing the filtered
    traces and, if you want, the plot of the filter used.
    The implemented filters are:
    
    -rect,
    -Hann,
    -Nuttall
    -Tukey (needs alpha parameter)
    (can be easily extended, see the code for extensions).
    
    Filter tool contains an envelope detector, that can be
    activated by the demodulation parameter in config file.
    
    Adaptive filter generator instead is a tool that generates
    an adaptive filter taking as input a set of dpacalc traces,
    producing as output a .dat file that contains the filter.
    In order to pass the generated filter to the filter program,
    use the filterFile parameter in the config file.
    
    IMPORTANT: the fftw3 library that computes the DFT and IDFT
    was compiled passing --enable-float parameter to the 
    configure script. In this way the library supports 
    computation with floating numbers. If you don't configure 
    in that way, the compiler will NOT resolve the function 
    calls correctly. If you want to use the double estension instead,
    then find in the code the string fftwf and replace with fftw.
    To compile correctly:
    download the FFTW3 from http://www.fftw.org/download.html
    extract the archive and enters the fftw3 directory
    ./configure --enable-single --enable-float
    make
    make install
    
    Usage:
    ./filter.out config_file
    ./adaptive_filter_generator.out config_file

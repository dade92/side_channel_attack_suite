    trace inspector inspects dpacalc traces.
    
    Produces gnuplot scripts (.gpl files) to show the mean
    the dev standard of the traces and the first n traces,
    a .png file with the persistence graph and the spectrum
    of the first trace.
    
    It takes as input a config file with the needed parameters.
    An example can be found in config_example. In the config file
    you will find different useful parameters to set the output
    image; during my thesis project I found these parameters quite
    important, looking the traces or the spectrum carefully usually
    brings clever deductions.
    
    IMPORTANT: the fftw3 library that computes the DFT and IDFT
    was compiled passing --enable-float parameter to the 
    configure script. In this way the library supports 
    computation with floating numbers. If you don't configure 
    in that way, the compiler will NOT resolve the function 
    calls correctly. If you want to use the double estension instead,
    then find in the code the string fftwf and replace with fftw.
    To compile correctly:
    download the FFTW3 from http://www.fftw.org/download.html
    extract the archive, enters the fftw3 directory and run:
    ./configure --enable-float
    make
    make install

    Usage: 
    ./traceInspector.out configFile

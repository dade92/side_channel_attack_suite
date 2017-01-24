    This is an analysis tool that performs correlation power analysis (CPA).
    To compile you only need to give a "make" command.

    The analysis tool takes as input a config file; an example of the 
    config file format can be found in config_example.

    As output the tool gives gnuplot scripts (.gpl files) 
    with respective .dat files and a log file that 
    contains the statistics of the considered intervals. It puts the output 
    script and .dat files in a directory because it could produce
    a lot of files.

    Regarding the power models, there are three of them:
    hamming weight, hamming distance and hamming distance sum (the sum of the hamming distances)

    Regarding the considered input in each interval, there is a distinction:

    --  if it is an AES attack, that is, if the size of the key is greater than 0, then 
        the positions are:
            ptx     take the simple plaintext.
            ar      addround,take ptx^key.
            sub     subbyte, take SBOX[ptx^key].
            
    --  if it is a known input attack, that is, if the key size is 0, then the positions
        are:
            ptxY_Z, where Y and Z are integer numbers, to take the Y input and Z input;
            for instance, ptx1_2 will take the first and the second input passed.
            
            addptxY_Z take the Y and Z input and compute the sum among them.
            
            xorptxY_Z take the Y and Z input and compute the xor among them.
            
            andptxY_Z take the Y and Z input and compute the and among them.
            
            mulptxY_Z take the Y and Z input and compute the mul among them.
            
            other operations of the same form: <op>ptxX_Y.
    Usage:
    ./analysis.out config_file
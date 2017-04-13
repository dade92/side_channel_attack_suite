    Side channel analysis suite:
    
    Copyright (C) 2016  Davide Botti

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>    

    Dependencies: boost c++ libraries, fftw3
    About fftw3: they were compiled enabling float API. So go to the fftw3 directory and run
                ./configure --enable-float
                make
                make install
    About boost libraries: you have only to install by apt-get.

    The suite contains:

    1) Architectural analysis tool performs side channel attacks on a set of traces in dpacalc format. 
    It can also perform known input analysis, useful for architectural characterization.

    2) Trace inspector tool produces png images with the mean and the standard deviation of the traces.
    It draws also the persistence graph of the traces and their spectrum (based on the mean).

    3) T-test tool performs t-test analysis on a pair of traces, of order 1 and higher (up to order 5,
    actually is useless implement attacks of order higher than that).
    
    4) Filter tool, used to filter raw traces and to generate an adaptive filter from a set of 
    dpacalc traces (can demodulate the signal if necessary).
    
    5) Trace aligner tool, used to align traces that could be not well aligned when acquired, and a splitter tool
    that, given a single trace made up of different AES computations, try to split into different traces,
    in which every trace correspond to an AES encryption.

    All of these tools accept as argument a config file, usage:
    ./tool_name.out configFile
    Each directory has a config file example with detailed information about all the parameters, a README.md file that explain how the tool works and its dependencies and a Makefile to compile the submodule separately (be careful because all these tools use the classes in common directory) which contains a target (clean_images) to remove all the images in the directory.

    All of these tools produce gnuplot scripts that should be passed to gnuplot in order to 
    generate the .png images or a .dat file of filtered/aligned traces.
    
    There is a Makefile that will compile everything in this directory.
    Just type Make in the terminal. If you want to add a macro-module to the suite,
    create a new sub-directory, create a Makefile for that sub-module (copy and paste
    from the other sub-modules the Makefile and change the parameters), then change
    the root Makefile (the one in this directory) adding the new sub-directory to the 
    param SUBDIRS.
    Common directory does not contain any executable, but a series of classes and functions widely used in the suite.
    
    Up to now, the tool well support float input files, while double input files were not used.
    
    Git will ignore all .out,.o,.dat,files, all the directories called "output" or "configuration" 
    and all the images (.png and .svg) and gnuplot scripts (.gpl files). So it is suggested to generate
    binary executable with the  extension .out, and to create directories with the aforementioned 
    name if you don't want to track something.

    Side channel analysis tools:
    
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

    Dependencies: boost c++ libraries, libpng.

    1) Architectural analysis tool performs side channel attacks on a set of traces in dpacalc format. 
    It can also perform known input analysis, useful for architectural characterization.

    2) Trace inspector produces png images with the mean and the standard deviation of the traces.
    It draws also the persistence graph of the traces.

    3) T-test tool performs t-test analysis on a pair of traces, of order 1 and higher (up to order 5,
        actually is useless implement attacks of order higher than that).

    All of these tools accept as argument a config file. 
    Each directory has a config file example with detailed information about all the parameters.

    All of these tools produce gnuplot scripts that should be passed to gnuplot in order to generate the .png images.
    
    There is a Makefile that will compile everything in this directory. Just type Make in the terminal.
    
    Git will ignore all .out,.o,.dat,files, all the directories called "output" or "configuration" 
    and all the images (.png and .svg) and gnuplot scripts (.gpl files). So it is suggested to generate binary executable with the 
    extension .out, and to create directories with the aforementioned name if you don't want to track
    something.

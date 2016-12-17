Side channel analysis tools by Davide Botti:

dependencies: boost c++ libraries, libpng, gnuplot.

1) Architectural analysis tool performs side channel attacks on a set of traces in dpacalc format. It can also perform known input analysis, useful for architectural characterization.

2) Trace inspector produces png images with the mean and the standard deviation of the traces. It draws also the persistence graph of the traces.

3) T-test tool performs t-test analysis on a pair of traces, of order 1 and higher (up to order 5).

All of these tools accept as argument a config file; each directory has a config file example with detailed information about all the parameters.

All of these tools produce gnuplot scripts that should be passed to gnuplot in order to generate the -png images.

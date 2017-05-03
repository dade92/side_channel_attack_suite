CC=g++
CFLAGS=-O3 -march=native -std=c++11 -lm -lfftw3f -g3
#CFLAGS=-std=c++11 -lm
COMMONS=../common/input.o ../common/math.o ../common/output.o
OBJECTS=config.o realigner.o traceSplitter.o trace_aligner.o ../aes/aes.o

all:trace_aligner.out

trace_aligner.out:$(COMMONS) $(OBJECTS)
	$(CC) $^ $(CFLAGS) -o $@
	
%.o: %.cpp
	$(CC) -c $< $(CFLAGS) -o $@

clean:
	rm *.o *.out
clean_images:
	find . -name "*.gpl" -type f|xargs rm -f
	find . -name "*.dat" -type f|xargs rm -f
	find . -name "*.png" -type f|xargs rm -f
	find . -name "*.tex" -type f|xargs rm -f
	find . -name "*.eps" -type f|xargs rm -f

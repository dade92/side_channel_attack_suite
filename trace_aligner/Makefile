CC=g++
CFLAGS=-O3 -march=native -std=c++11 -lm -lfftw3f
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

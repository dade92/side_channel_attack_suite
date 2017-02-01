CC=g++
CFLAGS=-O3 -march=native -std=c++11 -lm -lfftw3f
COMMONS=../common/input.o ../common/math.o
OBJECTS=config.o output.o transform.o filter.o

all:filter.out

filter.out:$(COMMONS) $(OBJECTS)
	$(CC) $^ $(CFLAGS) -o $@

%.o: %.cpp
	$(CC) -c $< $(CFLAGS) -o $@

clean:
	rm *.o *.out
CC=g++
CFLAGS=-O3 -march=native -std=c++11 -lm -lfftw3f
#CFLAGS= -std=c++11 -lm -lfftw3f
COMMONS=../common/input.o ../common/math.o
OBJECTS=config.o output.o transform.o filter.o
ADAPTIVE_FILTER_GENERATOR_OBJECTS=filter_generator_config.o adaptive_filter_generator.o

all:filter.out adaptive_filter_generator.out

filter.out:$(COMMONS) $(OBJECTS)
	$(CC) $^ $(CFLAGS) -o $@

adaptive_filter_generator.out:$(COMMONS) $(ADAPTIVE_FILTER_GENERATOR_OBJECTS)
	$(CC) $^ $(CFLAGS) -o $@
	
%.o: %.cpp
	$(CC) -c $< $(CFLAGS) -o $@

clean:
	rm *.o *.out

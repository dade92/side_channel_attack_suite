#CFLAGS=-g3 -std=c++11 -lpthread 

CFLAGS=-O3 -march=native -std=c++11 -lpthread -lfftw3f
CC=g++
COMMONS=../common/input.o ../common/math.o
OBJECTS=config.o powermodel.o statisticalAnalysis.o output.o analysis.o 

all: analysis.out

analysis.out: $(COMMONS) $(OBJECTS)
	$(CC) -o $@ $^ $(CFLAGS)

%.o: %.cpp
	$(CC) -o $@ -c $< $(CFLAGS)

clean_images:
	rm output/*.gpl
	rm output/*.dat
	rm output/*.png
clean:
	rm *.o *.out


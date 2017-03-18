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
	find . -name "*.gpl" -type f|xargs rm -f
	find . -name "*.dat" -type f|xargs rm -f
	find . -name "*.png" -type f|xargs rm -f
	find . -name "*.tex" -type f|xargs rm -f
	find . -name "*.eps" -type f|xargs rm -f
clean:
	rm *.o *.out


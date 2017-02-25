#CFLAGS=-g3 -std=c++11 -lpthread -lpng

CFLAGS=-O3 -march=native -std=c++11 -lpthread -lpng -lfftw3f -lm
CC=g++
OBJECTS=config.o trace_inspector.o
COMMONS=../common/input.o
 
all:trace_inspector.out

trace_inspector.out:$(COMMONS) $(OBJECTS)
	$(CC) -o $@ $^ $(CFLAGS)

%.o:%.cpp
	$(CC) -o $@ -c $< $(CFLAGS)
	
clean:
	rm *.o
	rm *.out
clean_images:
	find . -name "*.gpl" -type f|xargs rm -f
	find . -name "*.dat" -type f|xargs rm -f
	find . -name "*.png" -type f|xargs rm -f

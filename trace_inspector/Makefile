#CFLAGS=-g3 -std=c++11 -lpthread -lpng

CFLAGS=-O3 -march=native -std=c++11 -lpthread -lpng
CC=g++
OBJECTS=config.o traceInspector.o
COMMONS=../common/input.o
 
all:traceInspector.out

traceInspector.out:$(COMMONS) $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

%.o:%.cpp
	$(CC) $(CFLAGS) -o $@ -c $<
	
clean:
	rm *.o
	rm *.out
clean_images:
	rm *.gpl
	rm *.dat
	rm *.png

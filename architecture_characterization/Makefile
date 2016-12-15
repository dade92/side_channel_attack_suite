#CFLAGS=-g3 -std=c++11 -lpthread 

CFLAGS=-O3 -march=native -std=c++11 -lpthread

OBJECTS=config.o input.o powermodel.o statisticalAnalysis.o output.o analysis.o 

all: analysis.out

analysis.out: $(OBJECTS)
	g++ $(CFLAGS) -o $@ $^	
	
statisticalAnalysis.out: $(DEBUG_OBJECTS_STAT)
	g++ $(CFLAGS) -o $@ $^

%.o: %.cpp
	g++ $(CFLAGS) -o $@ -c $< 

clean_images:
	rm output/*.gpl
	rm output/*.dat
	rm output/*.png
clean:
	rm *.o *.out


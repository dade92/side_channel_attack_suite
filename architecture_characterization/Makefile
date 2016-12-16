#CFLAGS=-g3 -std=c++11 -lpthread 

CFLAGS=-O3 -march=native -std=c++11 -lpthread

COMMONS=../common/input.o

OBJECTS=config.o powermodel.o statisticalAnalysis.o output.o analysis.o 

all: analysis.out

analysis.out: $(COMMONS) $(OBJECTS)
	g++ $(CFLAGS) -o $@ $^

%.o: %.cpp
	g++ $(CFLAGS) -o $@ -c $<
../common/%.o:../common/%.cpp
	g++ $(CFLAGS) -o $@ -c $<

clean_images:
	rm output/*.gpl
	rm output/*.dat
	rm output/*.png
clean:
	rm *.o *.out


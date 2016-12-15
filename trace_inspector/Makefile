OBJECTS=input.o config.o traceInspector.o
#CFLAGS=-g3 -std=c++11 -lpthread -lpng

CFLAGS=-O3 -march=native -std=c++11 -lpthread -lpng
 
all:traceInspector.out


traceInspector.out:$(OBJECTS)
	g++ $(CFLAGS) -o $@ $^

%.o:%.cpp
	g++ $(CFLAGS) -o $@ -c $<
	
clean:
	rm *.o
	rm *.out
clean_images:
	rm *.gpl
	rm *.dat
	rm *.png

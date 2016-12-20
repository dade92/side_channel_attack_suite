#CFLAGS=-g3 -std=c++11 -lpthread

CFLAGS=-O3 -march=native -std=c++11 -lpthread -lm

OBJECTS=input.o math.o

all:$(OBJECTS)
	
%.o:%.cpp
	g++ $(CFLAGS) -o $@ -c $<
	
clean:
	rm *.o
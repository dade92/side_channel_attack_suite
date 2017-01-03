#CFLAGS=-g3 -std=c++11 -lpthread

CFLAGS=-O3 -march=native -std=c++11 -lpthread -lm
CC=g++
OBJECTS=input.o math.o

all:$(OBJECTS)
	
%.o:%.cpp
	$(CC) $(CFLAGS) -o $@ -c $<
	
clean:
	rm *.o

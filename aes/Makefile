#CFLAGS=-g3 -std=c++11 -lpthread

CFLAGS=-O3 -march=native -std=c++11 -lpthread -lm
CC=g++
OBJECTS=aes_ttable.o aes_test.o aes.o

all:aes_test.out

aes_test.out:$(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^
	
%.o:%.cpp
	$(CC) $(CFLAGS) -o $@ -c $<
	
clean:
	rm *.o 
	rm *.out

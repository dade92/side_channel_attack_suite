#CFLAGS=-g3 -std=c++11 -lpthread

CFLAGS=-O3 -march=native -std=c++11 -lpthread -lm
CC=g++
OBJECTS=config.o output.o tTest.o
COMMONS=../common/input.o ../common/math.o
 
all:tTest.out

tTest.out:$(COMMONS) $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

%.o:%.cpp
	$(CC) $(CFLAGS) -o $@ -c $<
	
clean:
	rm *.o
	rm *.out
clean_images:
	find . -name "*.gpl" -type f|xargs rm -f
	find . -name "*.dat" -type f|xargs rm -f
	find . -name "*.png" -type f|xargs rm -f

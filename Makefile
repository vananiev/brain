
CC = g++
CFLAGS = -Wall -I/home/vitek/.doc/Documents/Projects/brain/include
LDFLAGS = 

all: brain

brain: brain.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

brain.o: brain.cpp
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.out *.o main

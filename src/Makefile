
CC = g++
CFLAGS = -Wall -I/home/vitek/.doc/Documents/Projects/brain/include
LDFLAGS = -lpthread

all: john

john: john.o brain.o cell.o synapse.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

%.o: %.cpp
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.out *.o john

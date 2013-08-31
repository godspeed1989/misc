CC = g++
CPPFLAGS = -Wall

all: test

bwt.o: bwt.cpp bwt.hpp
test.o: test.cpp

test: test.o bwt.o
	$(CC) $+ -o $@

clean:
	rm -rf *.o test


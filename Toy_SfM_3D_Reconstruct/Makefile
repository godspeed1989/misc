
INCLUDE = `pkg-config --cflags opencv`
LIBS +=  -lm
LIBS += `pkg-config --libs opencv gtk+-2.0`

CXXFLAGS = -Wall -g

all: main

main: main.cpp
	g++ $(CXXFLAGS) $(INCLUDE) $+ -o $@ $(LIBS)

clean:
	rm -rf *.o

.PHONY: main

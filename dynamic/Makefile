CC = g++
LIBS += `xml2-config --libs`
CPPFLAGS = -Wall `xml2-config --cflags` -g

TARGET = dynamic

all: $(TARGET)

main.o: main.cpp
xmlreader.o: xmlreader.cpp xmlreader.hpp xmlreader_static.hpp

$(TARGET): xmlreader.o main.o
	$(CC) $+ $(LIBS) -o $@

clean:
	rm -rf *.o $(TARGET)


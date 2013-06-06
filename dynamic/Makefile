CC = g++
LIBS += `xml2-config --libs`
CPPFLAGS = -Wall `xml2-config --cflags` -g

TARGET = dynamic

all: $(TARGET)

main.o: main.cpp
xmlreader.o: xmlreader.cpp xmlreader.hpp xmlreader_static.hpp
filereader.o: filereader.cpp filereader.hpp filereader_static.hpp xmlreader.o

$(TARGET): xmlreader.o filereader.o main.o
	$(CC) $+ $(LIBS) -o $@

clean:
	rm -rf *.o $(TARGET)


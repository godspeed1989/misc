CC = g++
AR = ar
LIBS += `xml2-config --libs`
CPPFLAGS = -Wall `xml2-config --cflags` -g

TARGET = dynamic

all: $(TARGET)

main.o: main.cpp
endian.o: endian.cpp endian.hpp
xmlreader.o: xmlreader.cpp xmlreader.hpp xmlreader_static.hpp
filereader.o: filereader.cpp filereader.hpp filereader_static.hpp xmlreader.o

lib$(TARGET).a: xmlreader.o filereader.o endian.o
	$(AR) -cq $@ $+

$(TARGET): main.o lib$(TARGET).a
	$(CC) $< $(LIBS) -L. -l$(TARGET) -o $@

clean:
	rm -rf *.o lib$(TARGET).a $(TARGET)


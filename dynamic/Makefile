CC = g++
LIBS += `xml2-config --libs`
CPPFLAGS = -Wall `xml2-config --cflags`

XML = xmlreader

all: $(XML)

$(XML).o: $(XML).cpp

$(XML): $(XML).o
	$(CC) $< $(LIBS) -o $@

clean:
	rm -rf *.o $(XML)


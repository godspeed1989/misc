CC = g++
TARGET = g_seq_align
CPPFLAGS = -Wall
LDFLAGS = -lc -lm

all: $(TARGET)

$(TARGET).o: $(TARGET).cpp

$(TARGET): $(TARGET).o

clean:
	rm -rf $(TARGET) $(TARGET).o


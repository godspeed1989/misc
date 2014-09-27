#
# Makefile for the trace utility.
#
# M. Tim Jones <mtj@mtjones.com>
#

CC = gcc

OBJS = trace.o symbols.o stack.o

all: pvtrace

pvtrace: $(OBJS)
	$(CC) -o $@ $(OBJS)

pvt_test: instrument.c pvt_test.c
	$(CC) -g -finstrument-functions -Wall $+ -o $@

.c.o:
	$(CC) $(CFLAGS) -Wall -c $<

install: pvtrace
	cp pvtrace /usr/local/bin

clean:
	rm -f pvtrace *.o trace.txt graph.dot pvt_test core core.*


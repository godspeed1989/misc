CC=gcc
CFLAGS= -g -Wall
RM=rm -f
ifeq ($(SHELL),sh.exe)
    RM := del /f/q
endif

all: test test_win32

tthread.o: tthread.c tthread.h
	$(CC) $(CFLAGS) -D_TTHREAD_POSIX_ -lpthread -c $< -o $@

tthread_win32.o: tthread.c tthread.h
	$(CC) $(CFLAGS) -D_TTHREAD_WIN32_ -c $< -o $@

test.o: test.c
	$(CC) $(CFLAGS) -D_TTHREAD_POSIX_ -lpthread -c $< -o $@

test_win32.o: test.c
	$(CC) $(CFLAGS) -D_TTHREAD_WIN32_ -c $< -o $@

test: test.o tthread.o
test_win32: test_win32.o tthread_win32.o

clean:
	$(RM) *.o test test.exe test_win32.exe

# flags
CC = g++
CFS = -Wall -msse
LFS = 

# args
RELEASE = 0
BITS =
CFLAGS =

#  make RELEASE=1.
ifeq ($(RELEASE),0)
	# debug
	CFS += -g
else
	# release
	CFS += -static -O3 -DNDEBUG
	LFS += -static
endif

# [args] 程序位数. 32代表32位程序, 64代表64位程序, 其他默认. make BITS=32.
ifeq ($(BITS),32)
	CFS += -m32
	LFS += -m32
else
	ifeq ($(BITS),64)
		CFS += -m64
		LFS += -m64
	else
	endif
endif

# [args] 使用 CFLAGS 添加新的参数. make CFLAGS="-mavx".
CFS += $(CFLAGS)

.PHONY : all clean

# files
TARGETS = testccpuid testccpuidc
OBJS = testccpuid.o ccpuid.o

all : $(TARGETS)

testccpuid : $(OBJS)
	$(CC) $(LFS) -o $@ $^


ccpuid.o : ccpuid.cpp ccpuid.hpp ccpuid.h
	$(CC) $(CFS) -c $<


testccpuid.o : testccpuid.cpp ccpuid.hpp ccpuid.h
	$(CC) $(CFS) -c $<


testccpuidc : testccpuidc.c ccpuid.h
	gcc $(CFS) -o $@ $<


clean :
	rm -f $(OBJS) $(TARGETS) $(addsuffix .exe,$(TARGETS))


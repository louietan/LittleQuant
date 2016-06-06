
CC=g++

CFLAGS=-g -Wall -fPIC

INCEX= ../../sdk/inc

LIB= -L ../../sdk/linux/linux64 \
     -lkstradeapi \
     -lkslkc64r
	 
ifeq ($(vtype),32r)
    CFLAGS= -O2  -m32 -fPIC
    LIB:=$(subst linux64,linux32,${LIB})
    LIB:=$(subst kslkc64r,kslkc32r,${LIB})
endif

ifeq ($(vtype),64r)
    CFLAGS= -O2 -fPIC
endif

all: TraderApi_Sample

TraderApi_Sample: TraderApi_Sample.o
	${CC} ${CFLAGS} -o $@ $^  ${LIB} 
TraderApi_Sample.o: TraderApi_Sample.cpp
	${CC} ${CFLAGS} -o $@ -c $^  -I ${INCEX}
 
clean:
	rm *.o TraderApi_Sample 

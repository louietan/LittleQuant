
CC=g++

CFLAGS=-D_DEBUG -g -Wall -fPIC -lrt

INCEX= ../../sdk/inc

LIB= -L ../../sdk/linux/linux64 \
     -lksmarketdataapi \
     -lkslkc64r

ifeq ($(vtype),32r)
    CFLAGS= -O2  -m32 -fPIC
    LIB:=$(subst lnx64,lnx32,${LIB})
    LIB:=$(subst kslkc64r,kslkc32r,${LIB})
endif

ifeq ($(vtype),64r)
    CFLAGS= -O2 -fPIC
endif

all: MdUserApi_Sample

MdUserApi_Sample: MdUserApi_Sample.o
	${CC} ${CFLAGS} -o $@ $^  ${LIB} 
MdUserApi_Sample.o: MdUserApi_Sample.cpp
	${CC} ${CFLAGS} -o $@ -c $^ -I ${INCEX}
clean:
	rm *.o MdUserApi_Sample 

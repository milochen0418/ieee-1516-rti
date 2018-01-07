CC = g++
CFLAGS = -Wall -O2  -I../include -I../src/rti/ -I../src/fedex/
OBJECTS = ../src/rti/RTIambImplementor.o \
		../src/rti/MsgHandler.o \
		../src/rti/Endecode.o \
		../src/rti/PubSubTable.o \
		../src/rti/OMTDebug.o


LDFLAGS = -lpthread -L../lib -lms -lomt

all:
	@make -C ../src/
	$(CC) $(CFLAGS) -o test test.cc $(OBJECTS) $(LDFLAGS) 
	$(CC) $(CFLAGS) -o encode encode.cc $(OBJECTS) $(LDFLAGS) 

clean:
	@make clean -C ../src
	rm -f test test.o encode encode.o

.PHONY: all clean


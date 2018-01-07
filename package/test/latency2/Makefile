CC = g++
CFLAGS = -Wall -O2  -I../../include -I../../src/rti/ -I../../src/fedex/ -g
OBJECTS = ../../src/rti/RTIambImplementor.o \
		../../src/rti/MsgHandler.o \
		../../src/rti/Endecode.o \
		../../src/rti/PubSubTable.o \
		../../src/rti/OMTDebug.o


LDFLAGS = -lpthread -L../../lib -lms -lomt

all:
	@make -C ../../src/
	$(CC) $(CFLAGS) -o FederateA FederateA.cc $(OBJECTS) $(LDFLAGS) 
	$(CC) $(CFLAGS) -o FederateB FederateB.cc $(OBJECTS) $(LDFLAGS) 

clean:
	@make clean -C ../../src
	rm -f FederateA FederateA.o FederateB FederateB.o

.PHONY: all clean


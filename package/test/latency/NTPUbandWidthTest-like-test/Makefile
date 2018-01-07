RTIDIR = ../../../
CC = g++
CFLAGS = -Wall -O2  -I$(RTIDIR)include -I$(RTIDIR)src/rti/ -I$(RTIDIR)src/fedex/ -g
OBJECTS = $(RTIDIR)src/rti/RTIambImplementor.o \
		$(RTIDIR)src/rti/MsgHandler.o \
		$(RTIDIR)src/rti/Endecode.o \
		$(RTIDIR)src/rti/PubSubTable.o \
		$(RTIDIR)src/rti/OMTDebug.o


LDFLAGS = -lpthread -L$(RTIDIR)lib -lms -lomt

all:
	@make -C $(RTIDIR)src/
	$(CC) $(CFLAGS) -o FederateA FederateA.cc $(OBJECTS) $(LDFLAGS) 
	$(CC) $(CFLAGS) -o FederateB FederateB.cc $(OBJECTS) $(LDFLAGS) 

clean:
	@make clean -C $(RTIDIR)src
	rm -f FederateA FederateA.o FederateB FederateB.o

.PHONY: all clean


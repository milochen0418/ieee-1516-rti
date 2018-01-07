CC = gcc
CPP = g++
CFLAGS = -Wall -O2 -DDEBUG -D__NET_TCP__ -I../ms/net -I../ms/net/tcp -I../ms/utils -I../ms/message -I../../include 

VSEROBJS=../ms/message/vserver.o

UTILOBJS = ../ms/utils/debug.o \
	   ../ms/utils/profile.o \
	   ../ms/utils/memory_util.o \
	   ../ms/utils/uuid_gen.o \
	   ../ms/utils/list.o 

NETOBJS = ../ms/net/tcp/sock.o \
	  ../ms/net/tcp/tcp.o \
	  ../ms/net/net.o 

OTHEROBJS = ../rti/Endecode.o \
	    ../rti/PubSubTable.o


%.o: %.c
	$(CC) $(CFLAGS) $(UTILFLAGS) $< -c -o $@
%.o: %.cc
	$(CPP) $(CFLAGS) $(UTILFLAGS) $< -c -o $@	

all: $(UTILOBJS) $(NETOBJS) $(VSEROBJS) $(OTHEROBJS)  
	$(CPP) $(CFLAGS) -o fedex fedex.cc $(VSEROBJS) $(UTILOBJS) $(NETOBJS) $(OTHEROBJS)  -lpthread -L../../lib -lomt
clean:
	rm -f $(UTILOBJS) $(NETOBJS) $(VSEROBJS) $(OTHEROBJS)  fedex

.PHONY: all clean
../ms/message/vserver.o: ../ms/message/msgdef.h
../ms/utils/debug.o: ../ms/utils/debug.h
../ms/utils/profile.o: ../ms/utils/profile.h ../ms/utils/memory_util.h
../ms/utils/memory_util.o: ../ms/utils/memory_util.h ../ms/utils/debug.h
../ms/utils/uuid_gen.o: ../ms/utils/uuid_gen.h
../ms/utils/list.o: ../ms/utils/list.h ../ms/utils/debug.h ../ms/utils/memory_util.h
../ms/net/tcp/sock.o: ../ms/net/tcp/sock.h ../ms/utils/debug.h
../ms/net/tcp/tcp.o: ../ms/net/tcp/tcp.h ../ms/utils/debug.h ../ms/utils/memory_util.h ../ms/utils/uuid_gen.h ../ms/utils/list.h ../ms/net/tcp/sock.h
../ms/net/net.o: ../ms/net/net.h ../ms/utils/debug.h ../ms/net/tcp/tcp.h


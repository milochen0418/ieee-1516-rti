CC = gcc
CPP = g++
CFLAGS = -Wall -O2 -DDEBUG -D__NET_TCP__ -I./net -I./net/tcp -I./message  -I./utils  -I../../include

MSGOBJS = ms-funcs.o 

UTILOBJS = utils/debug.o \
	   utils/profile.o \
	   utils/memory_util.o \
	   utils/uuid_gen.o \
	   utils/list.o 

NETOBJS = net/tcp/sock.o \
	  net/tcp/tcp.o \
	  net/net.o 

OTHEROBJ = ../rti/Endecode.o

%.o: %.c
	$(CC) $(CFLAGS) $(UTILFLAGS) $< -c -o $@
%.o: %.cc
	$(CPP) $(CFLAGS) $(UTILFLAGS) $< -c -o $@

all: $(UTILOBJS) $(NETOBJS) $(MSGOBJS) $(OTHEROBJ)
	@ar cru libms.a $(UTILOBJS) $(NETOBJS) $(MSGOBJS) ms-funcs.o
	ranlib libms.a
	mv libms.a ../../lib/
clean:
	@make clean -C message
	rm -f ../../lib/libms.a
	rm -f $(UTILOBJS) $(NETOBJS) $(MSGOBJS) fedex

.PHONY: all clean

utils/debug.o: utils/debug.h
utils/profile.o: utils/profile.h utils/memory_util.h
utils/memory_util.o: utils/memory_util.h utils/debug.h
utils/uuid_gen.o: utils/uuid_gen.h
utils/list.o: utils/list.h utils/debug.h utils/memory_util.h
net/tcp/sock.o: net/tcp/sock.h utils/debug.h
net/tcp/tcp.o: net/tcp/tcp.h utils/debug.h utils/memory_util.h utils/uuid_gen.h utils/list.h net/tcp/sock.h
net/net.o: net/net.h utils/debug.h net/tcp/tcp.h
ms-funcs.o: ms-funcs.h ms-types.h


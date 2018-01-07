CC = gcc
CPP=gcc
CFLAGS =  -DDEBUG -D__NET_TCP__ -I../net -I../net/tcp -I../utils

OBJECTS = ../net/net.o
LDFLAGS =
OBJECTS += ../net/tcp/tcp.o \
	   ../net/tcp/sockio.o
LDFLAGS += -L../net/tcp
OBJECTS += ../utils/debug.o\
	   ../utils/profile.o \
	   ../utils/memory_util.o \
	   ../utils/uuid_gen.o \
	   ../utils/list.o
LDFLAGS += -L../utils -lpthread

all: $(OBJECTS)
	@$(CPP) $(CFLAGS) -c vserver.c

clean:
	rm -f msg.o vserver.o $(OBJECTS)

CC = g++
CFLAGS = -Wall -DDEBUG -O2 -I../../include/ -I. -I../fedex/ -I../ms/utils/

OBJECTS = OMTDebug.o PubSubTable.o Endecode.o RTIambImplementor.o MsgHandler.o

%.o: %.cc
	$(CC) $(CFLAGS) $(UTILFLAGS) $< -c -o $@

all: $(OBJECTS)

clean:
	rm -f $(OBJECTS)

.PHONY: all clean


PubSubTable.o: PubSubTable.h ../ms/utils/debug.h
RTI_RTIambassador.o: RTI_RTIambassador.h RTI_Exceptions.h RTI_SpecificConfig.h RTI_bool.h \
			RTI_Typedefs.h RTI_SpecificTypedefs.h ../ms/utils/debug.h macro.h
Endecode.o: Endecode.h ../ms/utils/debug.h
MsgHandler.o: MsgHandler.h

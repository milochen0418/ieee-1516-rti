# Project: OMT Parser

CPP  = g++
CC   = gcc
RES  = 
OBJ  =  Heap.o StrictCounter.o String.o Templates.o  $(RES)
LINKOBJ  = Heap.o StrictCounter.o String.o Templates.o  $(RES)
LIBDIR = ../../lib/
#LIBDIR = 
LIBS =   
INCS = 
CXXINCS = 
SHAREDLIB  = libomt.so
CXXFLAGS = $(CXXINCS)  -fPIC -I../rti
CFLAGS = $(INCS) 

.PHONY: all clean

all: $(SHAREDLIB)
	cp $(SHAREDLIB) $(LIBDIR)

install:
	cp $(SHAREDLIB) $(LIBDIR)
	ln -sf $(LIBDIR)$(SHAREDLIB).1.0 $(LIBDIR)$(SHAREDLIB) 
	ln -sf $(LIBDIR)$(SHAREDLIB).1.0 $(LIBDIR)$(SHAREDLIB).1 
	ldconfig -n $(LIBDIR)

clean: 
	rm -f *.o *.cpp~ *.h~ *.so
	rm -f $(LIBDIR)$(SHAREDLIB)

$(SHAREDLIB): $(OBJ)
	$(CPP) $(LINKOBJ) -shared -o "libomt.so" $(LIBS)

Templates.o: Templates.cpp
	$(CPP) -c Templates.cpp -o Templates.o $(CXXFLAGS)

Heap.o: Heap.cpp
	$(CPP) -c Heap.cpp -o Heap.o $(CXXFLAGS)


#omtparser1.o: omtparser1.cpp
#	$(CPP) -c omtparser1.cpp -o omtparser1.o $(CXXFLAGS)

StrictCounter.o: StrictCounter.cpp
	$(CPP) -c StrictCounter.cpp -o StrictCounter.o $(CXXFLAGS)

String.o: String.cpp
	$(CPP) -c String.cpp -o String.o $(CXXFLAGS)

#OHT.o: OHT.cpp
#	$(CPP) -c OHT.cpp -o OHT.o $(CXXFLAGS)

#UniqueStringCollection.o: UniqueStringCollection.cpp
#	$(CPP) -c UniqueStringCollection.cpp -o UniqueStringCollection.o $(CXXFLAGS)


#ifndef STRICTCOUNTER_H
#define STRICTCOUNTER_H

#include "Heap.h"

namespace omt{

class StrictCounter
{
public:

    StrictCounter();

    ~StrictCounter();

    unsigned long requestID();

    void releaseID(unsigned long );

protected:
    MaxHeap *_maxheap;  //after constructor be invoked, _maxheap is be builded.
    unsigned long _maxcount;//after constructor be invoked, _maxcount=1;
};


}
#endif

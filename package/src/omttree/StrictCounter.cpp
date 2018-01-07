#ifndef STRICTCOUNTER_CPP
#define STRICTCOUNTER_CPP

#include "StrictCounter.h"
#include "Heap.h"

namespace omt{

StrictCounter::StrictCounter()
{
    //建立一MaxHeap物件，預設大小為1000
    _maxheap = new MaxHeap ;
    _maxcount = 1;
    _maxheap->insert(_maxcount);
}

StrictCounter::~StrictCounter()
{
    delete  _maxheap;
}

unsigned long StrictCounter::requestID()
{

    //判斷MaxHeap可使用元素是否為零
    if(_maxheap->isEmpty())
    {
        _maxheap->insert(++_maxcount);
        return(_maxheap->extract());
    }
    else
    {
        //從MaxHeap取出一最大的ID
        return(_maxheap->extract());
    }
}


void StrictCounter::releaseID(unsigned long theID)
{
    //歸還的ID必須大於0且是目前在StrictCount._maxcount記錄內的
    if(theID !=0 && theID  <= _maxcount)
    {
        _maxheap->insert(theID);
    }
    else
        throw 0;
}

}
#endif





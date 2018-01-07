#ifndef STRICTCOUNTER_CPP
#define STRICTCOUNTER_CPP

#include "StrictCounter.h"
#include "Heap.h"

namespace omt{

StrictCounter::StrictCounter()
{
    //�إߤ@MaxHeap����A�w�]�j�p��1000
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

    //�P�_MaxHeap�i�ϥΤ����O�_���s
    if(_maxheap->isEmpty())
    {
        _maxheap->insert(++_maxcount);
        return(_maxheap->extract());
    }
    else
    {
        //�qMaxHeap���X�@�̤j��ID
        return(_maxheap->extract());
    }
}


void StrictCounter::releaseID(unsigned long theID)
{
    //�k�٪�ID�����j��0�B�O�ثe�bStrictCount._maxcount�O������
    if(theID !=0 && theID  <= _maxcount)
    {
        _maxheap->insert(theID);
    }
    else
        throw 0;
}

}
#endif





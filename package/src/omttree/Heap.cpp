#ifndef HEAP_CPP
#define HEAP_CPP

#include <iostream>

#include "Heap.h"

namespace omt{

MaxHeap::MaxHeap()
{
    m_Size = DEFAULT_HEAP_SIZE;
    m_Array = new unsigned long[DEFAULT_HEAP_SIZE];
    m_Counter = 0;

    //initialization
    for(int i = 0; i <= m_Size; i++)
    {
        m_Array[i] = 0;
    }
}

MaxHeap::~MaxHeap()
{
    delete [] m_Array;
}


unsigned long MaxHeap::extract()
{
    unsigned long RootValue;
    int currentIndex, leftIndex, rightIndex, Index;

    //已無元素可提供使用
    if(m_Counter == 0)
    {
        reSize(m_Size * 2);
    }

    //樹根
    currentIndex = 1;
    //左節點
    leftIndex = (currentIndex * 2);
    //右節點
    rightIndex = (currentIndex * 2) + 1;

    //取出樹根值
    RootValue = m_Array[currentIndex];

    //清除樹根值
    m_Array[currentIndex] = 0;

    while(leftIndex < m_Counter)
    {
        if(rightIndex >= m_Counter)
        {
            Index = leftIndex;
        }
        else
        {
            if(m_Array[rightIndex] > m_Array[leftIndex])
            {
                Index = rightIndex;
            }
            else
            {
                Index = leftIndex;
            }
        }

        if(m_Array[Index] > m_Array[currentIndex])
        {
            swap(&m_Array[currentIndex], &m_Array[Index]);
            currentIndex = Index;
            leftIndex = currentIndex * 2;
            rightIndex = leftIndex + 1;
        }
        else
            break;
    }

    //可使用元素減1
    m_Counter--;

    return RootValue;
}

void MaxHeap::insert(unsigned long theValue)
{
    unsigned long Counter;

    Counter = m_Counter+1;

    //陣列大小不足時，調整陣列大小為二倍
    if(Counter > m_Size)
    {
        reSize(m_Size * 2);
    };

    //可使用元素加一
    m_Counter++;

    int currentIndex = m_Counter;

    m_Array[currentIndex] = theValue;

    //計算父節點的陣列索引
    int parentIndex = (currentIndex)/2;

    //若小於0，則無父節點
    while(parentIndex != 0)
    {
        //若目前節點的值>父節點的值，則交換兩節點的值，且再往上層檢查
        if(m_Array[currentIndex] > m_Array[parentIndex])
        {
            swap(&m_Array[currentIndex], &m_Array[parentIndex]);
            currentIndex = parentIndex;
            parentIndex = currentIndex/2;
        }
        else
            break;
    }
}


//交換Heap元素內容
void MaxHeap::swap(unsigned long *theX, unsigned long *theY)
{
    unsigned long temp;

    temp = *theX;
    *theX = *theY;
    *theY = temp;
}

//判斷MaxHeap是否為空, True:空, False:非空
bool MaxHeap::isEmpty(void)
{
    return((m_Counter==0) ? true : false);
}

//取得目前已使用的Heap
int MaxHeap::getCounter(void)
{
    //傳回目前MaxHeap可使用元素個數
    return m_Counter;
}

void  MaxHeap::reSize(int newSize)
{
    unsigned long *newArray = new unsigned long [newSize];
    for(int i = 0; i <= m_Size; i++)
    {
        newArray[i] = m_Array[i];
    }
    delete [] m_Array;

    m_Array = newArray;

    //陣列可使用元素對= 新陣列大小-原資料大小
    m_Counter = newSize - m_Size;
    m_Size = newSize;
}

} //end of namespace omt
#endif

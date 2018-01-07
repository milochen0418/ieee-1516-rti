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

    //�w�L�����i���Ѩϥ�
    if(m_Counter == 0)
    {
        reSize(m_Size * 2);
    }

    //���
    currentIndex = 1;
    //���`�I
    leftIndex = (currentIndex * 2);
    //�k�`�I
    rightIndex = (currentIndex * 2) + 1;

    //���X��ڭ�
    RootValue = m_Array[currentIndex];

    //�M����ڭ�
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

    //�i�ϥΤ�����1
    m_Counter--;

    return RootValue;
}

void MaxHeap::insert(unsigned long theValue)
{
    unsigned long Counter;

    Counter = m_Counter+1;

    //�}�C�j�p�����ɡA�վ�}�C�j�p���G��
    if(Counter > m_Size)
    {
        reSize(m_Size * 2);
    };

    //�i�ϥΤ����[�@
    m_Counter++;

    int currentIndex = m_Counter;

    m_Array[currentIndex] = theValue;

    //�p����`�I���}�C����
    int parentIndex = (currentIndex)/2;

    //�Y�p��0�A�h�L���`�I
    while(parentIndex != 0)
    {
        //�Y�ثe�`�I����>���`�I���ȡA�h�洫��`�I���ȡA�B�A���W�h�ˬd
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


//�洫Heap�������e
void MaxHeap::swap(unsigned long *theX, unsigned long *theY)
{
    unsigned long temp;

    temp = *theX;
    *theX = *theY;
    *theY = temp;
}

//�P�_MaxHeap�O�_����, True:��, False:�D��
bool MaxHeap::isEmpty(void)
{
    return((m_Counter==0) ? true : false);
}

//���o�ثe�w�ϥΪ�Heap
int MaxHeap::getCounter(void)
{
    //�Ǧ^�ثeMaxHeap�i�ϥΤ����Ӽ�
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

    //�}�C�i�ϥΤ�����= �s�}�C�j�p-���Ƥj�p
    m_Counter = newSize - m_Size;
    m_Size = newSize;
}

} //end of namespace omt
#endif

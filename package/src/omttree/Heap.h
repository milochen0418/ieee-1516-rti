#ifndef HEAP_H
#define HEAP_H

#define DEFAULT_HEAP_SIZE 100

namespace omt{

class MaxHeap
{
private:
    int m_Counter;  //�O���i�ϥΤ����ƥ�
    int m_Size;   //�O��Max Heap�ŧi�����ƶq
    unsigned long* m_Array; //MaxHeap�}�C����
    void swap(unsigned long *, unsigned long *);
public:
    MaxHeap();
    ~MaxHeap();

    unsigned long extract(void);
    void insert(unsigned long);
    bool isEmpty(void);
    int getCounter(void);
    void reSize(int);
};

}

#endif

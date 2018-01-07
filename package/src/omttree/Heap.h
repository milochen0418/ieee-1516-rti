#ifndef HEAP_H
#define HEAP_H

#define DEFAULT_HEAP_SIZE 100

namespace omt{

class MaxHeap
{
private:
    int m_Counter;  //記錄可使用元素數目
    int m_Size;   //記錄Max Heap宣告元素數量
    unsigned long* m_Array; //MaxHeap陣列指標
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

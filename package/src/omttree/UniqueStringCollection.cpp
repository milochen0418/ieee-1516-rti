#ifndef UNIQUESTRINGCOLLECTION_CPP
#define UNIQUESTRINGCOLLECTION_CPP

#include "UniqueStringCollection.h"

namespace omt
{


template <class T>
UniqueStringCollection<T>::UniqueStringCollection(int size):table_size(size)
{
    table = new Cell*[table_size];

    //initial all Cell to 0
    for(int i = 0; i < size; table[i++] = 0);

    n_entries = 0;
}

template <typename T>
UniqueStringCollection<T>::~UniqueStringCollection()
{
    destory_table(table, table_size);
}

//**********************************************************************
//* Method    :isExist
//* Purpose   :Check that whether theString is repeatly in UniqueStringCollection
//* Input     :theString
//* Output    :True
//*            False
//**********************************************************************
template <typename T>
inline bool UniqueStringCollection<T>::isExist(char* theString)
{
    Cell *cp = find_cell(theString);

    //若找到，傳回data。若找不到則傳回NULL
    return(cp ? true : false);
}


template <typename T>
void UniqueStringCollection<T>::insert(char* theString, type_ptr theData)
{
    add(theString, theData);
}


template <typename T>
void UniqueStringCollection<T>::cancel(char *theString)
{
    //計算theString的hash值
    unsigned int slot = hash(theString);

    //指向Cell的進入點
    Cell *cp_prev = table[slot];

    //若未找到slot，代表未找到theString
    if(cp_prev == 0) return;

    //若找到的字串為串列中的第一個元素
    if(strcmp(cp_prev->key, theString) == 0)
    {
        table[slot] = cp_prev->next;
        //刪除Cell
        delete cp_prev;
        --n_entries;
        return;
    }

    //走訪串列所有元素，如找到theString，刪除Cell，並維護單向串列指標
    for(Cell *cp = cp_prev->next; cp != 0; cp_prev = cp_prev->next, cp = cp->next)
    {
        if(strcmp(cp->key, theString) == 0)
        {
            cp_prev->next = cp->next;
            delete cp;
            --n_entries;
            return;
        }
    }
}

template <typename T>
typename UniqueStringCollection<T>::type_ptr UniqueStringCollection<T>::query(char *theString)
{
    Cell *cp = find_cell(theString);
    //若找到，傳回data。若找不到則傳回NULL
    if(cp != 0)
        return (T*) cp->data;
    else
        return NULL;

}

template <typename T>
unsigned long UniqueStringCollection<T>::size()
{
    return (unsigned long)n_entries;
}


//*****************************************************************************
// Cell implementation
//*****************************************************************************
/*template <typename T>
unsigned long UniqueStringCollection<T>::hash_function(char *theKey)
{
    unsigned long int HashValue, g;
    const char *str = theKey;

    // Compute the hash value for the given string.

    HashValue = 0;
    while (*str != '\0')
    {
        HashValue <<= 4;
        HashValue += (unsigned long) *str++;
        g = HashValue & ((unsigned long) 0xf << (32 - 4));
        if (g != 0)
        {
            HashValue ^= g >> (32 - 8);
            HashValue ^= g;
        }
    }
    return HashValue;

}*/

template <typename T>
void UniqueStringCollection<T>::add(char* theKey, type_ptr theData)
{

    //在Cell中尋找theKey字串,若找到則直接返回
    if(find_cell(theKey) != 0) return;

    //串列長度大於5，增加table
    if((n_entries/table_size) >= 5)
        expand_table();

    //計算字串的hash
    unsigned int slot = hash(theKey);

    //新增一Cell，且將theKey加入Cell
    if(table[slot] == 0)
        table[slot] = new Cell(theData, theKey);
    else
        table[slot] = new Cell(theData, theKey, table[slot]);
    ++n_entries;
}

template <typename T>
typename UniqueStringCollection<T>::Cell*  UniqueStringCollection<T>::find_cell(char *theKey)
{
    unsigned int slot = hash(theKey);

    //取得Cell的進入點
    Cell * cp = table[slot];

    while(cp != 0)
    {
        //走訪串列，找到theString，並傳回字串
        if(cp->key == theKey) return cp;
        cp = cp->next;
    }
    return cp;
}

template <typename T>
void UniqueStringCollection<T>::expand_table()
{
    Cell **old_table = table;
    unsigned int old_table_size = table_size;
    table_size *=2;
    n_entries = 0;
    table = new Cell * [table_size];

    for(int i = 0; i < table_size; table[i++] = 0)
    {
        for(int j = 0; j < old_table_size; j++)
        {
            Cell *cp, *cp_next;
            if((cp = old_table[j] )== 0) continue;
            else cp_next = cp->next;

            while(true)
            {
                add(cp->key, cp->data);
                if(cp_next == 0) break;
                cp = cp_next;
                cp_next = cp_next->next;
            }
        }
    }
    destory_table(old_table,old_table_size);
}

template <typename T>
void UniqueStringCollection<T>::destory_table(Cell ** dead_table, unsigned int dead_table_size)
{
    for(int i = 0; i< dead_table_size; i++)
    {
        Cell *cp, *cp_next;
        if((cp = dead_table[i]) == 0) continue;
        else cp_next=cp->next;

        while(true)
        {
            delete cp;
            if(cp_next == 0) break;
            cp = cp_next;
            cp_next = cp_next->next;
        }
    }
    delete dead_table;
}

//*****************************************************************************
//* hash(char *) computes a simple hash function
//*****************************************************************************
template <typename T>
unsigned int UniqueStringCollection<T>::hash(char *theKey)
{
    unsigned int return_value = 0;
    for(char * cp = theKey; *cp != 0; ++cp)
        return_value += *cp;
    return unsigned (return_value % table_size);
}


}


#endif

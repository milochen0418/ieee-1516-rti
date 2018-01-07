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

    //�Y���A�Ǧ^data�C�Y�䤣��h�Ǧ^NULL
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
    //�p��theString��hash��
    unsigned int slot = hash(theString);

    //���VCell���i�J�I
    Cell *cp_prev = table[slot];

    //�Y�����slot�A�N�����theString
    if(cp_prev == 0) return;

    //�Y��쪺�r�ꬰ��C�����Ĥ@�Ӥ���
    if(strcmp(cp_prev->key, theString) == 0)
    {
        table[slot] = cp_prev->next;
        //�R��Cell
        delete cp_prev;
        --n_entries;
        return;
    }

    //���X��C�Ҧ������A�p���theString�A�R��Cell�A�ú��@��V��C����
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
    //�Y���A�Ǧ^data�C�Y�䤣��h�Ǧ^NULL
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

    //�bCell���M��theKey�r��,�Y���h������^
    if(find_cell(theKey) != 0) return;

    //��C���פj��5�A�W�[table
    if((n_entries/table_size) >= 5)
        expand_table();

    //�p��r�ꪺhash
    unsigned int slot = hash(theKey);

    //�s�W�@Cell�A�B�NtheKey�[�JCell
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

    //���oCell���i�J�I
    Cell * cp = table[slot];

    while(cp != 0)
    {
        //���X��C�A���theString�A�öǦ^�r��
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

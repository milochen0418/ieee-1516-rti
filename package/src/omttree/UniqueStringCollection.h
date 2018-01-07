#ifndef UNIQUESTRINGCOLLECTION_H
#define UNIQUESTRINGCOLLECTION_H

//#pragma interface

#ifndef NULL
#define NULL __null
#endif

namespace omt
{

template <class T>
class UniqueStringCollection
{
    //We have know that UniqueStringCollection is set of string s.t. each string in it is unique
public:

    typedef T  type_value;
    typedef T* type_ptr;

    UniqueStringCollection(int size);

    ~UniqueStringCollection();

    bool isExist(char* theString);

    void insert(char* theString, type_ptr theData);

    void cancel(char* theString);

    unsigned long size();

    type_ptr query(char* theString);

private:

    struct Cell
    {

        const type_ptr data;

        //儲存計算過的hash值
        char *key;

        //指向下一個cell指標
        Cell *next;

        //cell的constructor
        Cell(type_ptr Cell_data, char * Cell_key, Cell * Cell_next = 0):data(Cell_data), key(Cell_key), next(Cell_next){}

    };

    Cell **table;
    unsigned int table_size;
    unsigned int n_entries;

    unsigned int hash(char *);
    void expand_table();
    void destory_table(Cell ** , unsigned int );
    //unsigned long hash_function(char *);

    void add(char* theString, type_ptr theData);

    Cell* find_cell(char *);


};

//#include "UniqueStringCollection.cpp"

}
#endif

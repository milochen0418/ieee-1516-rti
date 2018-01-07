#ifndef STRING_H
#define STRING_H

#include <iostream>
#include <iomanip>

namespace omt{

class String;
//non-member function
std::istream& operator>>(std::istream&, String&);
std::ostream& operator<<(std::ostream&, String&);

class String
{
public:
    //Constructor
    String();
    String(const char*);
    String(const String&);

    //Destructor
    ~String();

    String& append(const char* str, int len);
    String& assign(const char* str, int len);
    void reserve (int cap);

    // Return the length of a XmlString
    //- Comment by Bruce at 2006/11/30
    //int length ();
    int size(){return _size;}
    char* c_str(){return _string;}


    String& operator=(const String&);
    String& operator=(const char* );

    bool operator==(const String&);
    bool operator==(char*);
    char& operator[](int);

    String& operator += (const char * suffix);
    // += operator. Maps to append
    String& operator += (char single);
    // += operator. Maps to append
    String& operator += (const String & suffix);

private:
    //private attribute
    int   _capacity; //��ڰ}�C�j�p
    int	  _size;     //�r�����
    char  *_string;  //�r����СA���V��ڦr���}

    //private method
    int capacity(){return _capacity;}//�Ǧ^�}�C�j�p
};

}
#endif

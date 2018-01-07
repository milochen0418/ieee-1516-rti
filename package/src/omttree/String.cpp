#ifndef STRING_CPP
#define STRING_CPP

#include <cstring>
#include <iostream>
#include <iomanip>

#include "String.h"

namespace omt{

String::String()
{
    _size = 0;
    _capacity = _size+1;
    _string = new char[_capacity];

    memset(_string, 0, _capacity);
}


String::String( const char *str)
{

    _size = strlen(str);
    _capacity = _size+1; // +1是為了儲存字串終端字元'\0'
    _string = new char[_capacity];

    memset(_string, 0, _capacity);
    memmove(_string, str, _size);
}


String::String(const String &rhs)
{
    if(this != &rhs)
    {
        _size = rhs._size;
        _capacity = _size + 1;
        _string = new char[_capacity];

        memset(_string, 0 , _capacity);
        memcpy(_string, rhs._string, _size );
    }
}

String::~String()
{
    delete [] _string;
}


//len不包含終端字元'\0'
String& String::append (const char* str, int len)
{
    int newSize = _size + len;

    if(newSize >= _capacity)
    {
        reserve(newSize*2);
    }

    memcpy((_string +_size), str, len);
    _size = newSize;
    return *this;
}

//只改變capacity及_string指標
void String::reserve (int newCapacity)
{
    char *newString  = new char[newCapacity];

    memset(newString, 0, newCapacity);
    memcpy(newString, _string, _size);

    char *tmp = _string;
    _string = newString;

    newString = tmp;
    _capacity = newCapacity;

    delete [] tmp;
}

String& String::assign(const char* str, int len)
{
    if(len >= _capacity)
    {
        reserve(len*2);
    }
    _size = 0;
    append(str, len);
}

String& String::operator=(const String &rhs)
{
    //To avoid self copy
    if(this != &rhs)
    {
        delete [] _string;
        _size = rhs._size;
        _capacity = _size + 1;
        _string = new char[_capacity];
        memset(_string, 0, _capacity);
        strcpy(_string, rhs._string);
    }
    return *this;
}

String& String::operator=(const char *str)
{

    _size = strlen(str);
    _capacity = _size+1;
    delete [] _string;
    _string = new char[_capacity];
    memset(_string, 0, _capacity);
    memcpy(_string, str,_size);
    return *this;
}

bool String::operator==(const String &rhs )
{
    if(_size != rhs._size)
        return false;
    else
        return (!strcmp(_string, rhs._string) ? true : false);
}

bool String::operator==(char *str)
{
    return (!strcmp(_string, str) ? true : false);
}

char& String::operator[](int element)
{
    return _string[element];
}

std::istream& operator>>(std::istream& is, String &str)
{
    const int lmt_str_size = 1024;
    char inBuf[lmt_str_size];
    is >> std::setw(lmt_str_size) >> inBuf;
    str = inBuf;

    return is;
}

std::ostream& operator<<(std::ostream& os, String &rhs)
{
    return os << rhs.c_str();
}

String& String::operator += (const char *str)
{
    return append(str, strlen(str));
}

String& String::operator += (char ch)
{
    return append(&ch, 1);
}


String& String::operator += (const String &rhs)
{
    return append(rhs._string, rhs._size);
}

//- Comment by Bruce at 2006/11/30
//int String::length()
//{
//    return _size;
//}

}

#endif

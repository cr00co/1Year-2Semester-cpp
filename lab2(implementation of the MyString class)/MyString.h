#pragma once
#include <iostream>

class MyString {
private:
    char* data;
    int length;
    
public:
    MyString();
    MyString(const char* str);
    MyString(const MyString& other);
    
    ~MyString();
    
    MyString& operator=(const MyString& other);
    
    MyString operator+(const MyString& other) const;
    bool operator==(const MyString& other) const;
    char& operator[](int index);
    
    const char* getString() const { return data; }
    int getLength() const { return length; }
    
    friend std::ostream& operator<<(std::ostream& os, const MyString& str);
};
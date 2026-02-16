#include "MyString.h"
#include <cstring>

MyString::MyString() {
    length = 0;
    data = new char[1];
    data[0] = '\0';
}

MyString::MyString(const char* str) {
    if (str) {
        length = strlen(str);
        data = new char[length + 1];
        strcpy(data, str);
    } else {
        length = 0;
        data = new char[1];
        data[0] = '\0';
    }
}

MyString::MyString(const MyString& other) {
    length = other.length;
    data = new char[length + 1];
    strcpy(data, other.data);
}

MyString::~MyString() {
    delete[] data;
}

MyString& MyString::operator=(const MyString& other) {
    if (this == &other) {
        return *this;
    }
    
    delete[] data;
    
    length = other.length;
    data = new char[length + 1];
    strcpy(data, other.data);
    
    return *this;
}

MyString MyString::operator+(const MyString& other) const {
    MyString result;
    delete[] result.data;
    
    result.length = this->length + other.length;
    result.data = new char[result.length + 1];
    
    strcpy(result.data, this->data);
    strcat(result.data, other.data);
    
    return result;
}

bool MyString::operator==(const MyString& other) const {
    if (this->length != other.length) {
        return false;
    }
    return strcmp(this->data, other.data) == 0;
}

char& MyString::operator[](int index) {
    if (index < 0 || index >= length) {
        static char dummy = '\0';
        return dummy;
    }
    return data[index];
}

std::ostream& operator<<(std::ostream& os, const MyString& str) {
    os << str.data;
    return os;
}
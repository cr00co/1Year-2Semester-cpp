#include "MyString.h"
#include <iostream>

int main() {
    
    std::cout << "1. Создание объектов:" << std::endl;
    MyString s1("Hello");
    MyString s2(" World");
    MyString s3;
    MyString s4("Hello");
    
    std::cout << "s1 = \"" << s1 << "\"" << std::endl;
    std::cout << "s2 = \"" << s2 << "\"" << std::endl;
    std::cout << "s3 = \"" << s3 << "\"" << std::endl;
    std::cout << "Длина s1: " << s1.getLength() << std::endl;
    std::cout << std::endl;
    
    std::cout << "2. Копирование:" << std::endl;
    MyString s5 = s1;
    std::cout << "s5 (копия s1) = \"" << s5 << "\"" << std::endl;
    std::cout << std::endl;
    
    std::cout << "3. Присваивание:" << std::endl;
    MyString s6;
    s6 = s2;
    std::cout << "s6 (после s6 = s2) = \"" << s6 << "\"" << std::endl;
    
    s1 = s1;
    std::cout << "s1 после присваивания самому себе = \"" << s1 << "\"" << std::endl;
    std::cout << std::endl;
    
    std::cout << "4. Использование перегруженных операторов:" << std::endl;
    
    std::cout << "4.1. Оператор сложения (+):" << std::endl;
    MyString s7 = s1 + s2;
    std::cout << "s1 + s2 = \"" << s7 << "\"" << std::endl;
    
    MyString s8 = s1 + MyString(" ") + s2 + MyString("!");
    std::cout << "Цепочка сложения: \"" << s8 << "\"" << std::endl;
    std::cout << std::endl;
    
    std::cout << "4.2. Оператор сравнения (==):" << std::endl;
    std::cout << "s1 == s4? " << (s1 == s4 ? "да" : "нет") << std::endl;
    std::cout << "s1 == s2? " << (s1 == s2 ? "да" : "нет") << std::endl;
    std::cout << std::endl;
    
    std::cout << "4.3. Оператор индексации ([]):" << std::endl;
    std::cout << "s1[0] = '" << s1[0] << "'" << std::endl;
    std::cout << "s1[1] = '" << s1[1] << "'" << std::endl;
    std::cout << "s1[4] = '" << s1[4] << "'" << std::endl;
    
    s1[0] = 'h';
    std::cout << "После s1[0] = 'h': s1 = \"" << s1 << "\"" << std::endl;
    std::cout << std::endl;
    
    std::cout << "5. Дополнительные проверки:" << std::endl;
    std::cout << "C-строка s1: " << s1.getString() << std::endl;
    
    return 0;
}
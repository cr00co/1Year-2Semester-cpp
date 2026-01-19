#include <iostream>

class Counter 
{
public:
    // Конструктор по умолчанию
    Counter() 
    {
        value = 0;
    }

    // Параметризированный конструктор
    Counter(int startValue) 
    {
        if (startValue < 0) {
            value = 0;
        }
        else {
            value = startValue;
        }
    }

    // Деструктор
    ~Counter() 
    {
        std::cout << "Счетчик удален, финальное значение: " << value << std::endl;
    }

    // Методы
    void increment() {
        value++;
    }

    void decrement() {
        if (value > 0) {
            value--;
        }
    }

    int getValue() const {
        return value;
    }

    void reset() {
        value = 0;
    }

    void setValue(int newValue) {
        if (newValue < 0) {
            value = 0;
        }
        else {
            value = newValue;
        }
    }

    void doubleValue() {
        value *= 2;
    }

    bool isZero() const {
        return value == 0;
    }

    std::string getValueAsString() const {
        return std::to_string(value);
    }

private:
    int value;
};

int main() 
{
    // Создание объектов класса
    Counter counter1;
    Counter counter2(444);

    // Вывод значений объектов
    std::cout << "Текущее значение counter1: " << counter1.getValue() << std::endl;
    std::cout << "Текущее значение counter2: " << counter1.getValue() << std::endl;

    // Несколько оперций над каждым объектом
    for (int i = 0; i < 5; i++) 
    {
        counter1.increment();
    }
    counter1.decrement();

    counter2.reset();
    counter2.setValue(5555);
    counter2.doubleValue();

    // Вывод новых значений
    std::cout << "" << std::endl;
    std::cout << "Значение counter1 после некоторых операций: " << counter1.getValue() << std::endl;
    std::cout << "Значение counter2 после некоторых операций: " << counter2.getValue() << std::endl;
    std::cout << "" << std::endl;


    return 0;
}
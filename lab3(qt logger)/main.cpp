#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include "Logger.hpp"

class MainWindow : public QWidget {
public:
    MainWindow() {
        Logger::info("Приложение запущено");
        Logger::debug("Инициализация главного окна");
        
        QPushButton *button1 = new QPushButton("Нормальная кнопка");
        QPushButton *button2 = new QPushButton("Кнопка с ошибкой");
        
        QVBoxLayout *layout = new QVBoxLayout;
        layout->addWidget(button1);
        layout->addWidget(button2);
        setLayout(layout);
        
        connect(button1, &QPushButton::clicked, []() {
            Logger::info("Нормальная кнопка нажата");
        });
        
        connect(button2, &QPushButton::clicked, []() {
            Logger::warning("Нажата кнопка с ошибкой - потенциально опасное действие");
            Logger::error("Ошибка: попытка выполнения опасной операции", __FILE__, __LINE__);
            Logger::fatal("Критическая ошибка: разыменование nullptr!", __FILE__, __LINE__);
            int* ptr = nullptr;
            *ptr = 42;
        });
        
        button1->setStyleSheet("QPushButton { background-color: green; color: white; }");
        button2->setStyleSheet("QPushButton { background-color: red; color: white; }");

        setWindowTitle("Вариант 3");
        resize(300, 200);
    }
};

int main(int argc, char *argv[]) {
    Logger::debug("Запуск приложения");
    QApplication app(argc, argv);
    
    MainWindow window;
    window.show();
    
    return app.exec();
}
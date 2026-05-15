#include <QApplication>
#include "MainWindow.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("QuoteApp");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("Lab7");

    MainWindow window;
    window.show();

    return app.exec();
}

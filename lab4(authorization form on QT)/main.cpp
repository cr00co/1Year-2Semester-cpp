#include <QApplication>
#include "AuthorizationForm.hpp"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    AuthorizationForm window;
    window.show();
    return app.exec();
}

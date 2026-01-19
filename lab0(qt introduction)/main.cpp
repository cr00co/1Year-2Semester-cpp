#include <QApplication>
#include <QPushButton>
#include <QLabel>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QLabel label1("Привет, мир!");
    label1.setWindowTitle("Лаб 0 Пухальский А. В.");
    label1.resize(400, 200);
    label1.setFixedSize(400, 200);
    label1.setStyleSheet("color: green; font-size: 20px;");
    label1.setAlignment(Qt::AlignCenter);
    label1.show();

    QLabel label2("Нажато раз:");
    label2.setParent(&label1);
    label2.setStyleSheet("color: orange; font-size: 20px;");
    label2.resize(200, 30);
    label2.move(15, 130);
    label2.show();

    QPushButton button ("0", &label1);
    button.setStyleSheet("color: orange; font-size: 16px;");
    button.resize(120, 30);
    button.move(140, 130);
    button.show();

    int clickCount = 0;
    QObject::connect(&button, &QPushButton::clicked, [&]() {
        clickCount++;
        button.setText(QString::number(clickCount));
    });

    QPixmap pixmap("/Users/anton/Pictures/cat_in_glasses.jpg");
    QLabel photoLabel(&label1);
    photoLabel.setPixmap(pixmap.scaled(120, 120, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    photoLabel.move(270, 10);
    photoLabel.show();

    return app.exec();
}

#include "MainWindow.hpp"
#include "Facade.hpp"
#include "Logger.hpp"
#include <QVBoxLayout>
#include <QPushButton>
#include <QTextEdit>
#include <QFileDialog>
#include <QMessageBox>
#include <QFont>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle("Lab 6 – Вариант 2");
    resize(700, 500);

    auto* central = new QWidget(this);
    setCentralWidget(central);

    auto* layout = new QVBoxLayout(central);

    m_parseButton = new QPushButton("Parse JSON", this);
    m_output      = new QTextEdit(this);
    m_output->setReadOnly(true);
    m_output->setFont(QFont("Courier New", 10));

    layout->addWidget(m_parseButton);
    layout->addWidget(m_output);

    connect(m_parseButton, &QPushButton::clicked, this, &MainWindow::onParseClicked);
}

void MainWindow::onParseClicked() {
    QString inputPath = QFileDialog::getOpenFileName(
        this, "Выберите warehouse.json", "", "JSON Files (*.json)"
    );
    if (inputPath.isEmpty()) return;

    QString outputPath = QFileDialog::getSaveFileName(
        this, "Сохранить результат как", "variant.json", "JSON Files (*.json)"
    );
    if (outputPath.isEmpty()) return;

    try {
        Logger::info("Starting processing...");
        Facade facade(inputPath.toStdString(), outputPath.toStdString());
        std::string result = facade.execute();
        m_output->setText(QString::fromStdString(result));
        Logger::info("Done.");
    } catch (const std::exception& e) {
        Logger::error(std::string("Exception: ") + e.what());
        QMessageBox::critical(this, "Ошибка", QString::fromUtf8(e.what()));
    }
}

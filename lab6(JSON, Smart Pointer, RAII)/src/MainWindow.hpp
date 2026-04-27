#pragma once
#include <QMainWindow>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QWidget>
#include <QFileDialog>
#include <QMessageBox>
#include <QString>

#include "Facade.hpp"
#include "Logger.hpp"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

private slots:
    void onParseClicked();

private:
    QPushButton* m_parseButton = nullptr;
    QTextEdit*   m_output      = nullptr;
};

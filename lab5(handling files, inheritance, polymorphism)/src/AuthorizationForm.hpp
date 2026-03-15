#pragma once
#include <QMainWindow>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QStackedWidget>
#include "FileHandlers.hpp"

class MainWindow;

// Taken from lab4 almost unchanged.
// Only difference: after successful login/register opens MainWindow
// instead of the old static menu widget.
class AuthorizationForm : public QMainWindow {
    Q_OBJECT
public:
    AuthorizationForm(QWidget* parent = nullptr);

private slots:
    void handleLogin();
    void handleRegister();
    void switchMode();
    void handleLogout();

private:
    void setupUI();
    void showError(const QString& msg);
    void clearFields();
    void openMainWindow(const QString& username);

    UserFileHandler  m_ufh;
    PhotoFileHandler m_pfh;

    QStackedWidget* stacked;
    QString         currentUser;
    bool            isLoginMode = true;

    // Widgets — same names as in lab4
    QLineEdit   *userInput, *passInput, *passConfirmInput;
    QPushButton *actionBtn, *switchBtn;
    QLabel      *errorLabel, *confirmLabel;

    MainWindow* m_mainWindow = nullptr;
};

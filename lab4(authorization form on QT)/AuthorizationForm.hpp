#pragma once

#include <QMainWindow>
#include <QStackedWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include "Database.hpp"

class AuthorizationForm : public QMainWindow {
    Q_OBJECT

public:
    AuthorizationForm(QWidget* parent = nullptr);
    ~AuthorizationForm() = default;

private slots:
    void showLoginForm();
    void showRegistrationForm();
    void handleLogin();
    void handleRegister();
    void handleLogout();

private:
    void setupUI();
    void showError(const QString& message);
    void clearInputs();

    QStackedWidget* stacked;
    Database database;
    QString currentUser;

    // Login widgets
    QLineEdit *loginUser, *loginPass;
    QPushButton *btnLogin, *btnToReg;
    QLabel *loginError;

    // Register widgets
    QLineEdit *regUser, *regEmail, *regPass, *regConfirm;
    QPushButton *btnRegister, *btnToLogin;
    QLabel *regError;

    // Menu widgets
    QLabel *menuWelcome, *menuInfo;
    QPushButton *btnLogout;
};

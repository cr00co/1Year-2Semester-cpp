#pragma once
#include <QMainWindow>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QStackedWidget>
#include "Database.hpp"

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
    void showSuccess(const QString& msg);
    void clearFields();

    Database database;
    QStackedWidget* stacked;
    QString currentUser;
    bool isLoginMode = true;

    QLineEdit *userInput, *passInput, *passConfirmInput;
    QPushButton *actionBtn, *switchBtn, *logoutBtn;
    QLabel *errorLabel, *infoLabel, *confirmLabel;
};

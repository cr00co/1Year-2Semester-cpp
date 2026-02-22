#include "AuthorizationForm.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QMessageBox>
#include "Logger.hpp"

AuthorizationForm::AuthorizationForm(QWidget* parent)
    : QMainWindow(parent), database("users.db") {
    
    Logger::info("Authorization Form initialized");
    
    QWidget* central = new QWidget(this);
    stacked = new QStackedWidget(central);
    
    QVBoxLayout* layout = new QVBoxLayout(central);
    layout->addWidget(stacked);
    layout->setContentsMargins(0, 0, 0, 0);
    setCentralWidget(central);
    
    setupUI();
    stacked->setCurrentIndex(0);
    
    setWindowTitle("Authorization");
    setGeometry(100, 100, 500, 350);
}

void AuthorizationForm::setupUI() {
    // ===== LOGIN FORM =====
    QWidget* loginW = new QWidget();
    QVBoxLayout* loginL = new QVBoxLayout(loginW);
    
    QLabel* loginTitle = new QLabel("Login");
    loginTitle->setStyleSheet("font-size: 18px; font-weight: bold;");
    loginL->addWidget(loginTitle);
    
    loginL->addWidget(new QLabel("Username:"));
    loginUser = new QLineEdit();
    loginUser->setPlaceholderText("Enter username");
    loginL->addWidget(loginUser);
    
    loginL->addWidget(new QLabel("Password:"));
    loginPass = new QLineEdit();
    loginPass->setPlaceholderText("Enter password");
    loginPass->setEchoMode(QLineEdit::Password);
    loginL->addWidget(loginPass);
    
    loginError = new QLabel("");
    loginError->setStyleSheet("color: red;");
    loginL->addWidget(loginError);
    
    QHBoxLayout* btnL = new QHBoxLayout();
    btnLogin = new QPushButton("Login");
    btnToReg = new QPushButton("Register");
    connect(btnLogin, &QPushButton::clicked, this, &AuthorizationForm::handleLogin);
    connect(btnToReg, &QPushButton::clicked, this, &AuthorizationForm::showRegistrationForm);
    btnL->addWidget(btnLogin);
    btnL->addWidget(btnToReg);
    
    loginL->addLayout(btnL);
    loginL->addStretch();
    stacked->addWidget(loginW);
    
    // ===== REGISTRATION FORM =====
    QWidget* regW = new QWidget();
    QVBoxLayout* regL = new QVBoxLayout(regW);
    
    QLabel* regTitle = new QLabel("Register");
    regTitle->setStyleSheet("font-size: 18px; font-weight: bold;");
    regL->addWidget(regTitle);
    
    regL->addWidget(new QLabel("Username (3-20 chars):"));
    regUser = new QLineEdit();
    regUser->setPlaceholderText("Username");
    regL->addWidget(regUser);
    
    regL->addWidget(new QLabel("Email:"));
    regEmail = new QLineEdit();
    regEmail->setPlaceholderText("Email");
    regL->addWidget(regEmail);
    
    regL->addWidget(new QLabel("Password (min 6, letters + digits):"));
    regPass = new QLineEdit();
    regPass->setPlaceholderText("Password");
    regPass->setEchoMode(QLineEdit::Password);
    regL->addWidget(regPass);
    
    regL->addWidget(new QLabel("Confirm:"));
    regConfirm = new QLineEdit();
    regConfirm->setPlaceholderText("Confirm password");
    regConfirm->setEchoMode(QLineEdit::Password);
    regL->addWidget(regConfirm);
    
    regError = new QLabel("");
    regError->setStyleSheet("color: red;");
    regL->addWidget(regError);
    
    QHBoxLayout* regBtnL = new QHBoxLayout();
    btnRegister = new QPushButton("Register");
    btnToLogin = new QPushButton("Back");
    connect(btnRegister, &QPushButton::clicked, this, &AuthorizationForm::handleRegister);
    connect(btnToLogin, &QPushButton::clicked, this, &AuthorizationForm::showLoginForm);
    regBtnL->addWidget(btnRegister);
    regBtnL->addWidget(btnToLogin);
    
    regL->addLayout(regBtnL);
    regL->addStretch();
    stacked->addWidget(regW);
    
    // ===== MAIN MENU =====
    QWidget* menuW = new QWidget();
    QVBoxLayout* menuL = new QVBoxLayout(menuW);
    
    menuWelcome = new QLabel("Welcome!");
    menuWelcome->setStyleSheet("font-size: 18px; font-weight: bold;");
    menuL->addWidget(menuWelcome);
    
    menuInfo = new QLabel("");
    menuInfo->setStyleSheet("font-size: 14px; margin: 20px 0;");
    menuL->addWidget(menuInfo);
    
    menuL->addStretch();
    btnLogout = new QPushButton("Logout");
    connect(btnLogout, &QPushButton::clicked, this, &AuthorizationForm::handleLogout);
    menuL->addWidget(btnLogout);
    
    stacked->addWidget(menuW);
}

void AuthorizationForm::handleLogin() {
    QString user = loginUser->text();
    QString pass = loginPass->text();
    
    if (user.isEmpty() || pass.isEmpty()) {
        Logger::warning("Login attempt with empty fields");
        showError("Fill all fields");
        return;
    }
    
    if (database.loginUser(user.toStdString(), pass.toStdString())) {
        Logger::info("User login successful: " + user.toStdString());
        currentUser = user;
        menuWelcome->setText("Welcome, " + user + "!");
        
        const User* u = database.getUser(currentUser.toStdString());
        if (u) {
            menuInfo->setText("Username: " + QString::fromStdString(u->username) +
                            "\nEmail: " + QString::fromStdString(u->email));
        }
        
        clearInputs();
        stacked->setCurrentIndex(2);
    } else {
        Logger::warning("Login failed: invalid credentials for " + user.toStdString());
        showError("Invalid username or password");
    }
}

void AuthorizationForm::handleRegister() {
    QString user = regUser->text();
    QString email = regEmail->text();
    QString pass = regPass->text();
    QString confirm = regConfirm->text();
    
    if (user.isEmpty() || email.isEmpty() || pass.isEmpty()) {
        showError("Fill all fields");
        return;
    }
    
    if (pass != confirm) {
        showError("Passwords don't match");
        return;
    }
    
    auto result = database.registerUser(user.toStdString(), email.toStdString(), pass.toStdString());
    
    switch (result) {
        case Database::RegResult::SUCCESS:
            Logger::info("User registered successfully: " + user.toStdString());
            QMessageBox::information(this, "Success", "Registration successful!");
            clearInputs();
            showLoginForm();
            break;
        case Database::RegResult::USERNAME_EXISTS:
            showError("Username already exists");
            break;
        case Database::RegResult::EMAIL_EXISTS:
            showError("Email already exists");
            break;
        case Database::RegResult::INVALID_USERNAME:
            showError("Invalid username");
            break;
        case Database::RegResult::INVALID_EMAIL:
            showError("Invalid email");
            break;
        case Database::RegResult::INVALID_PASSWORD:
            showError("Password too weak");
            break;
    }
}

void AuthorizationForm::handleLogout() {
    Logger::info("User logged out");
    currentUser.clear();
    clearInputs();
    stacked->setCurrentIndex(0);
    QMessageBox::information(this, "Logout", "Logged out successfully");
}

void AuthorizationForm::showLoginForm() {
    clearInputs();
    stacked->setCurrentIndex(0);
}

void AuthorizationForm::showRegistrationForm() {
    clearInputs();
    stacked->setCurrentIndex(1);
}

void AuthorizationForm::showError(const QString& message) {
    regError->setText(message);
    loginError->setText(message);
}

void AuthorizationForm::clearInputs() {
    loginUser->clear();
    loginPass->clear();
    loginError->setText("");
    
    regUser->clear();
    regEmail->clear();
    regPass->clear();
    regConfirm->clear();
    regError->setText("");
}

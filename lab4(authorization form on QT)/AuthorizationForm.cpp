#include "AuthorizationForm.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include "Logger.hpp"

AuthorizationForm::AuthorizationForm(QWidget* parent)
    : QMainWindow(parent), database("users.db") {
    
    Logger::info("Application started");
    
    QWidget* central = new QWidget(this);
    stacked = new QStackedWidget(central);
    QVBoxLayout* mainLayout = new QVBoxLayout(central);
    mainLayout->addWidget(stacked);
    setCentralWidget(central);
    
    setupUI();
    stacked->setCurrentIndex(0);
    
    setWindowTitle("Authorization");
    setGeometry(100, 100, 350, 300);
    setFixedSize(350, 300);
}

void AuthorizationForm::setupUI() {
    QWidget* authWidget = new QWidget();
    QVBoxLayout* authLayout = new QVBoxLayout(authWidget);
    
    errorLabel = new QLabel();
    errorLabel->setStyleSheet("color: red; font-weight: bold;");
    authLayout->addWidget(errorLabel);
    
    authLayout->addWidget(new QLabel("Login:"));
    userInput = new QLineEdit();
    authLayout->addWidget(userInput);
    
    authLayout->addWidget(new QLabel("Password:"));
    passInput = new QLineEdit();
    passInput->setEchoMode(QLineEdit::Password);
    authLayout->addWidget(passInput);
    
    confirmLabel = new QLabel("Confirm Password:");
    confirmLabel->hide();
    authLayout->addWidget(confirmLabel);
    passConfirmInput = new QLineEdit();
    passConfirmInput->setEchoMode(QLineEdit::Password);
    passConfirmInput->hide();
    authLayout->addWidget(passConfirmInput);
    
    actionBtn = new QPushButton("Auth");
    switchBtn = new QPushButton("Switch to Register");
    
    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->addWidget(actionBtn);
    btnLayout->addWidget(switchBtn);
    authLayout->addLayout(btnLayout);
    authLayout->addStretch();
    
    connect(actionBtn, &QPushButton::clicked, this, &AuthorizationForm::handleLogin);
    connect(switchBtn, &QPushButton::clicked, this, &AuthorizationForm::switchMode);
    
    stacked->addWidget(authWidget);
    
    QWidget* menuWidget = new QWidget();
    QVBoxLayout* menuLayout = new QVBoxLayout(menuWidget);
    
    infoLabel = new QLabel();
    infoLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #7B1FA2; text-align: center;");
    infoLabel->setAlignment(Qt::AlignCenter);
    menuLayout->addWidget(infoLabel);

    QLabel* avatarLabel = new QLabel();
    avatarLabel->setAlignment(Qt::AlignCenter);
    avatarLabel->setStyleSheet("background-color: #4CAF50; color: white; font-size: 48px; font-weight: bold; border-radius: 10px;");
    avatarLabel->setFixedSize(100, 100);
    avatarLabel->setText("👤");
    menuLayout->addWidget(avatarLabel, 0, Qt::AlignCenter);
    menuLayout->addStretch();
    
    logoutBtn = new QPushButton("Logout");
    menuLayout->addWidget(logoutBtn);
    
    connect(logoutBtn, &QPushButton::clicked, this, &AuthorizationForm::handleLogout);
    
    stacked->addWidget(menuWidget);
}

void AuthorizationForm::handleLogin() {
    showError("");

    QString user = userInput->text();
    QString pass = passInput->text();
    
    if (user.isEmpty() || pass.isEmpty()) {
        showError("Fill all fields!");
        return;
    }
    
    // Check if user exists
    const User* u = database.getUser(user.toStdString());
    if (!u) {
        Logger::warning("Auth failed: user doesn't exist");
        showError("User doesn't exist!");
        return;
    }
    
    // Check password
    if (database.loginUser(user.toStdString(), pass.toStdString())) {
        Logger::info("User logged in: " + user.toStdString());
        currentUser = user;
        
        infoLabel->setText("Добро пожаловать, " + QString::fromStdString(u->username) + "!");
        clearFields();
        stacked->setCurrentIndex(1);
    } else {
        Logger::warning("Auth failed: wrong password");
        showError("Wrong password!");
    }
}

void AuthorizationForm::handleRegister() {
    showError("");

    QString user = userInput->text();
    QString pass = passInput->text();
    QString confirm = passConfirmInput->text();
    
    if (user.isEmpty() || pass.isEmpty() || confirm.isEmpty()) {
        showError("Fill all fields!");
        return;
    }
    
    if (pass != confirm) {
        showError("Passwords don't match!");
        return;
    }

    if (user.length() < 3 || user.length() > 20) {
        showError("Login must be 3-20 characters long!");
        return;
    }
    
    if (pass.length() < 6) {
        showError("Password must be at least 6 characters!");
        return;
    }
    
    bool hasLetter = false, hasDigit = false;
    for (QChar c : pass) {
        if (c.isLetter()) hasLetter = true;
        if (c.isDigit()) hasDigit = true;
    }
    
    if (!hasLetter || !hasDigit) {
        showError("Password must contain both letters and digits!");
        return;
    }
    
    Database::RegResult result = database.registerUser(
        user.toStdString(), pass.toStdString()
    );
    
    switch (result) {
        case Database::RegResult::SUCCESS:
            Logger::info("User registered: " + user.toStdString());
            showSuccess("Registration successful!");
            switchMode();
            break;
        case Database::RegResult::USERNAME_EXISTS:
            Logger::warning("Registration failed: username exists");
            showError("Username already exists!");
            break;
        case Database::RegResult::INVALID_DATA:
            Logger::warning("Registration failed: invalid data");
            showError("Invalid username or password!");
            break;
    }
}

void AuthorizationForm::switchMode() {
    isLoginMode = !isLoginMode;
    errorLabel->clear();
    clearFields();
    
    if (isLoginMode) {
        confirmLabel->hide();
        passConfirmInput->hide();
        actionBtn->setText("Auth");
        switchBtn->setText("Switch to Register");
        disconnect(actionBtn, nullptr, this, nullptr);
        connect(actionBtn, &QPushButton::clicked, this, &AuthorizationForm::handleLogin);
    } else {
        confirmLabel->show();
        passConfirmInput->show();
        actionBtn->setText("Register");
        switchBtn->setText("Back to Auth");
        disconnect(actionBtn, nullptr, this, nullptr);
        connect(actionBtn, &QPushButton::clicked, this, &AuthorizationForm::handleRegister);
    }
}

void AuthorizationForm::handleLogout() {
    Logger::info("User logged out: " + currentUser.toStdString());
    currentUser.clear();
    clearFields();
    isLoginMode = true;
    stacked->setCurrentIndex(0);
    actionBtn->setText("Auth");
    switchBtn->setText("Switch to Register");
    confirmLabel->hide();
    passConfirmInput->hide();
    disconnect(actionBtn, nullptr, this, nullptr);
    connect(actionBtn, &QPushButton::clicked, this, &AuthorizationForm::handleLogin);
    QMessageBox::information(this, "Logout", "Logged out successfully!");
}

void AuthorizationForm::showError(const QString& msg) {
    errorLabel->setText(msg);
}

void AuthorizationForm::showSuccess(const QString& msg) {
    QMessageBox::information(this, "Success", msg);
}

void AuthorizationForm::clearFields() {
    userInput->clear();
    passInput->clear();
    passConfirmInput->clear();
    errorLabel->clear();
}

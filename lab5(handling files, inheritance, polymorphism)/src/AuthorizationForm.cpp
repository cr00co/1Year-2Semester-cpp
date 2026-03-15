#include "AuthorizationForm.hpp"
#include "MainWindow.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QCoreApplication>
#include "Logger.hpp"

AuthorizationForm::AuthorizationForm(QWidget* parent)
    : QMainWindow(parent)
    , m_ufh(QCoreApplication::applicationDirPath() + "/users.csv")
    , m_pfh(QCoreApplication::applicationDirPath() + "/photos")
{
    Logger::info("Application started");
    m_ufh.load();
    m_pfh.load();

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
}

void AuthorizationForm::handleLogin() {
    showError("");
    QString user = userInput->text();
    QString pass = passInput->text();

    if (user.isEmpty() || pass.isEmpty()) { showError("Fill all fields!"); return; }

    if (!m_ufh.exists(user)) {
        Logger::warning("Auth failed: user doesn't exist");
        showError("User doesn't exist!");
        return;
    }
    if (m_ufh.authenticate(user, pass)) {
        Logger::info("User logged in: " + user.toStdString());
        currentUser = user;
        clearFields();
        openMainWindow(user);
    } else {
        Logger::warning("Auth failed: wrong password");
        showError("Wrong password!");
    }
}

void AuthorizationForm::handleRegister() {
    showError("");
    QString user    = userInput->text();
    QString pass    = passInput->text();
    QString confirm = passConfirmInput->text();

    if (user.isEmpty() || pass.isEmpty() || confirm.isEmpty()) {
        showError("Fill all fields!"); return;
    }
    if (pass != confirm) { showError("Passwords don't match!"); return; }

    QString err = m_ufh.registerUser(user, pass);
    if (err.isEmpty()) {
        Logger::info("User registered: " + user.toStdString());
        switchMode();
        showError("");
    } else {
        Logger::warning("Registration failed: " + err.toStdString());
        showError(err);
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

    if (m_mainWindow) {
        stacked->removeWidget(m_mainWindow);
        delete m_mainWindow;
        m_mainWindow = nullptr;
    }

    actionBtn->setText("Auth");
    switchBtn->setText("Switch to Register");
    confirmLabel->hide();
    passConfirmInput->hide();
    disconnect(actionBtn, nullptr, this, nullptr);
    connect(actionBtn, &QPushButton::clicked, this, &AuthorizationForm::handleLogin);

    stacked->setCurrentIndex(0);
    setFixedSize(350, 300);
}

void AuthorizationForm::openMainWindow(const QString& username) {
    User* user = m_ufh.find(username);
    if (!user) return;

    m_mainWindow = new MainWindow(user, &m_ufh, &m_pfh);
    connect(m_mainWindow, &MainWindow::loggedOut, this, &AuthorizationForm::handleLogout);

    stacked->addWidget(m_mainWindow);
    stacked->setCurrentWidget(m_mainWindow);
    setFixedSize(500, 420);
}

void AuthorizationForm::showError(const QString& msg) { errorLabel->setText(msg); }

void AuthorizationForm::clearFields() {
    userInput->clear();
    passInput->clear();
    passConfirmInput->clear();
    errorLabel->clear();
}

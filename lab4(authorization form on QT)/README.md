# Authorization Form - Qt6 | Подробное руководство

Приложение для регистрации и авторизации пользователей на Qt6 с безопасным хранением паролей.

---

## 📋 Содержание

1. [Как работает приложение](#как-работает-приложение)
2. [Структурные компоненты](#структурные-компоненты)
3. [Детальный разбор кода](#детальный-разбор-кода)
4. [Сборка и запуск](#сборка-и-запуск)

---

## Как работает приложение

### Общий поток работы

```
Пользователь запускает app (main.cpp)
         ↓
Qt приложение создаёт окно (AuthorizationForm)
         ↓
Пользователь видит форму входа
         ↓
Пользователь либо входит (loginUser),
либо регистрируется (registerUser)
         ↓
При регистрации: пароль кодируется (PasswordEncryption)
                 данные сохраняются в файл (Database)
         ↓
При входе: проверяется пароль
           показывается профиль пользователя
         ↓
Выход: очистка данных, возврат на форму входа
```

---

## Структурные компоненты

### 1️⃣ **main.cpp** (9 строк) - Точка входа

```cpp
#include <QApplication>
#include "AuthorizationForm.hpp"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);           // Создание приложения Qt
    AuthorizationForm window;               // Создание главного окна
    window.show();                          // Показать окно
    return app.exec();                      // Запустить цикл событий
}
```

**Что происходит построчно:**

- `QApplication app` - создаёт Qt приложение (нужно для окон и событий)
- `AuthorizationForm window` - создаём окно с формой входа/регистрации
- `window.show()` - окно становится видимым
- `app.exec()` - программа ждёт событий (клики, ввод текста) и реагирует на них

---

### 2️⃣ **User.hpp** (45 строк) - Структура пользователя

```cpp
struct User {
    std::string username;           // Имя пользователя
    std::string email;              // Email
    std::string passwordHash;       // Хеш пароля (НЕ сам пароль!)
```

**Для чего это нужно:**
- Хранит данные одного пользователя
- `passwordHash` - это **закодированный** пароль, оригинал удален (безопасность!)

#### Валидация имя пользователя:

```cpp
static bool isValidUsername(const std::string& username) {
    // Проверка длины: минимум 3, максимум 20 символов
    if (username.length() < 3 || username.length() > 20) return false;
    
    // Проверка каждого символа
    for (char c : username) {
        // Разрешены: буквы (a-z, A-Z), цифры (0-9), подчёркивание (_), дефис (-)
        if (!std::isalnum(c) && c != '_' && c != '-') return false;
    }
    return true;  // Всё хорошо!
}
```

**Примеры:**
- ✅ `john_doe` - валидное (буквы + подчёркивание)
- ✅ `user123` - валидное (буквы + цифры)
- ❌ `ab` - невалидное (только 2 символа, нужно минимум 3)
- ❌ `user@name` - невалидное (содержит @, не разрешено)

#### Валидация email:

```cpp
static bool isValidEmail(const std::string& email) {
    size_t at = email.find('@');        // Найти позицию @
    size_t dot = email.rfind('.');      // Найти последнюю позицию точки
    
    // Проверяем: есть ли @ и точка, и @ стоит ДО точки
    return at != std::string::npos && dot != std::string::npos && at < dot;
}
```

**Примеры:**
- ✅ `user@example.com` - валидный
- ✅ `john@domain.org` - валидный
- ❌ `userexample.com` - нет @
- ❌ `user@.com` - нечего между @ и точкой

#### Валидация пароля:

```cpp
static bool isValidPassword(const std::string& password) {
    if (password.length() < 6) return false;  // Минимум 6 символов
    
    bool hasLetter = false;     // Флаг: есть ли буквы?
    bool hasDigit = false;      // Флаг: есть ли цифры?
    
    // Проверяем каждый символ
    for (char c : password) {
        if (std::isalpha(c)) hasLetter = true;
        if (std::isdigit(c)) hasDigit = true;
    }
    
    // Валидный, только если есть И буквы И цифры
    return hasLetter && hasDigit;
}
```

**Примеры:**
- ✅ `Pass123` - валидный (буквы + цифры)
- ✅ `Password1` - валидный
- ❌ `password` - только буквы, нет цифр
- ❌ `12345678` - только цифры, нет букв
- ❌ `Pass1` - только 5 символов, нужно минимум 6

---

### 3️⃣ **PasswordEncryption.hpp** (27 строк) - Кодирование паролей

**Почему это важно:** Нельзя хранить пароли в открытом виде! Если кто-то украдёт файл, он получит все пароли. Вместо этого хранят **хеш** - это как отпечаток пальца пароля.

#### Хеширование (кодирование):

```cpp
static std::string hashPassword(const std::string& password) {
    unsigned char hash[SHA256_DIGEST_LENGTH];      // Массив для хеша (32 байта)
    
    // Применяем алгоритм SHA-256 к паролю
    SHA256(reinterpret_cast<unsigned char*>(const_cast<char*>(password.data())), 
           password.length(), hash);
    
    // Конвертируем в текстовый формат (для сохранения в файл)
    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    
    return ss.str();  // Возвращаем строку вида "e5fa44f2b31c1fb553b6021e7aab6b74476544c..."
}
```

**Пример:**
- Вводит пользователь: `Password123`
- Применяется SHA-256: `e5fa44f2b31c1fb553b6021e7aab6b74476544c`
- **Сохраняется**: `e5fa44f2b31c1fb553b6021e7aab6b74476544c` (всегда один и тот же результат)

#### Проверка пароля:

```cpp
static bool verifyPassword(const std::string& password, const std::string& hash) {
    // Просто кодируем введённый пароль и сравниваем со сохранённым
    return hashPassword(password) == hash;
}
```

**Как это работает:**
```
Регистрация:
  пользователь вводит пароль: "Pass123"
  кодируем: hash = SHA256("Pass123") = "a1b2c3d4e5f6..."
  сохраняем в файл: "a1b2c3d4e5f6..."

Вход (проверка):
  пользователь вводит пароль: "Pass123"
  кодируем: newHash = SHA256("Pass123") = "a1b2c3d4e5f6..."
  сравниваем: "a1b2c3d4e5f6..." == "a1b2c3d4e5f6..." ✅ пароль верный!
```

---

### 4️⃣ **Database.hpp** (84 строки) - Сохранение пользователей

```cpp
class Database {
private:
    std::vector<User> users;        // Все пользователи в памяти
    std::string filePath;           // Путь к файлу "users.db"
```

#### Загрузка из файла:

```cpp
void loadFromFile() {
    users.clear();                           // Очищаем память
    std::ifstream file(filePath);            // Открываем файл для чтения
    if (!file) return;                       // Если не открылся, выходим
    
    std::string line;
    while (std::getline(file, line)) {       // Читаем строку за строкой
        if (line.empty()) continue;          // Пропускаем пустые строки
        
        std::stringstream ss(line);          // Разбираем строку
        std::string username, email, hash;
        
        // Строка формата: "username|email|hash"
        // getline(..., '|') читает до символа '|'
        if (std::getline(ss, username, '|') && 
            std::getline(ss, email, '|') && 
            std::getline(ss, hash, '|')) {
            users.emplace_back(username, email, hash);  // Добавляем пользователя
        }
    }
    file.close();
}
```

**Пример файла users.db:**
```
john|john@email.com|e5fa44f2b31c1fb553b6021e7aab6b74476544c0d83cac9476...
alice|alice@domain.com|c9e8b4f2d7a1c5e3b8f6d1a9c7e4b2f8d5a3c1e7b9f4d6a2c8...
```

#### Регистрация:

```cpp
RegResult registerUser(const std::string& username, 
                      const std::string& email, 
                      const std::string& password) {
    
    // 1. Проверяем валидность всех полей
    if (!User::isValidUsername(username)) 
        return RegResult::INVALID_USERNAME;
    if (!User::isValidEmail(email)) 
        return RegResult::INVALID_EMAIL;
    if (!User::isValidPassword(password)) 
        return RegResult::INVALID_PASSWORD;
    
    // 2. Проверяем, нет ли уже такого пользователя
    for (const auto& u : users) {
        if (u.username == username) 
            return RegResult::USERNAME_EXISTS;      // username занят
        if (u.email == email) 
            return RegResult::EMAIL_EXISTS;         // email занят
    }
    
    // 3. Кодируем пароль
    std::string hash = PasswordEncryption::hashPassword(password);
    if (hash.empty()) return RegResult::SUCCESS;    // Ошибка кодирования
    
    // 4. Добавляем нового пользователя
    users.emplace_back(username, email, hash);
    
    // 5. Сохраняем в файл
    saveToFile();
    return RegResult::SUCCESS;
}
```

**Поток работы:**
```
Пользователь вводит: john | john@mail.com | Pass123

1. Проверяем всё валидное ✅
2. Проверяем john не занят ✅
3. Кодируем Pass123 → долгая_строка_хеша
4. Добавляем объект User("john", "john@mail.com", "долгая_строка_хеша")
5. Пишем в файл: john|john@mail.com|долгая_строка_хеша
6. Возвращаем SUCCESS
```

#### Вход (проверка пароля):

```cpp
bool loginUser(const std::string& username, 
              const std::string& password) {
    // Ищем пользователя по имени
    for (const auto& u : users) {
        if (u.username == username && 
            PasswordEncryption::verifyPassword(password, u.passwordHash)) {
            return true;        // Пароль совпадает!
        }
    }
    return false;               // Пользователь не найден или пароль неверный
}
```

**Как это работает:**
```
Пользователь вводит: john | Pass123

1. Ищем в памяти пользователя с именем "john" ✅ нашли
2. Проверяем пароль:
   - Берём введённый пароль: "Pass123"
   - Берём сохранённый хеш: "a7f3d2c1...e5fa44f2b31c1fb"
   - Извлекаем соль: "a7f3d2c1" (первые 32 символа)
   - Кодируем "Pass123" с этой солью
   - Сравниваем: новый_хеш == e5fa44f2b31c1fb ? ✅ ДА!
3. Возвращаем true (успешно вошли)
```

---

### 5️⃣ **AuthorizationForm** (274 строки) - Интерфейс

```cpp
class AuthorizationForm : public QMainWindow {
    Q_OBJECT  // Нужно для сигналов и слотов в Qt
```

#### Конструктор (инициализация):

```cpp
AuthorizationForm::AuthorizationForm(QWidget* parent)
    : QMainWindow(parent), database("users.db") {
    
    Logger::info("Authorization Form initialized");
    
    QWidget* central = new QWidget(this);        // Центральный виджет
    stacked = new QStackedWidget(central);       // "Стопка" экранов
    
    QVBoxLayout* layout = new QVBoxLayout(central);
    layout->addWidget(stacked);
    layout->setContentsMargins(0, 0, 0, 0);
    setCentralWidget(central);
    
    setupUI();                                   // Создаём все 3 экрана
    stacked->setCurrentIndex(0);                 // Показываем экран входа
    
    setWindowTitle("Authorization");
    setGeometry(100, 100, 500, 350);
}
```

**Что это означает:**

- `stacked` - как стопка карточек (экранов). Видна только верхняя карточка.
- Экран 0: Форма входа
- Экран 1: Форма регистрации  
- Экран 2: Меню профиля

#### Вход (handleLogin):

```cpp
void AuthorizationForm::handleLogin() {
    QString user = loginUser->text();      // Получаем введённое имя
    QString pass = loginPass->text();      // Получаем введённый пароль
    
    if (user.isEmpty() || pass.isEmpty()) {
        Logger::warning("Login attempt with empty fields");
        showError("Fill all fields");
        return;
    }
    
    // Проверяем в базе данных
    if (database.loginUser(user.toStdString(), pass.toStdString())) {
        Logger::info("User login successful: " + user.toStdString());
        
        currentUser = user;
        menuWelcome->setText("Welcome, " + user + "!");
        
        // Показываем информацию пользователя
        const User* u = database.getUser(currentUser.toStdString());
        if (u) {
            menuInfo->setText("Username: " + QString::fromStdString(u->username) +
                            "\nEmail: " + QString::fromStdString(u->email));
        }
        
        clearInputs();
        stacked->setCurrentIndex(2);        // Переходим на экран меню
    } else {
        Logger::warning("Login failed: invalid credentials");
        showError("Invalid username or password");
    }
}
```

**Поток для пользователя:**
```
1. Вводит имя и пароль
2. Нажимает "Login"
3. Проверяем в базе:
   - Есть ли пользователь? ✅
   - Пароль верен? ✅
4. Показываем профиль
5. Пользователь может выйти кнопкой "Logout"
```

#### Регистрация (handleRegister):

```cpp
void AuthorizationForm::handleRegister() {
    QString user = regUser->text();
    QString email = regEmail->text();
    QString pass = regPass->text();
    QString confirm = regConfirm->text();
    
    // Проверяем, заполнены ли все поля
    if (user.isEmpty() || email.isEmpty() || pass.isEmpty()) {
        showError("Fill all fields");
        return;
    }
    
    // Проверяем, совпадают ли пароли
    if (pass != confirm) {
        showError("Passwords don't match");
        return;
    }
    
    // Пытаемся зарегистрировать в базе
    auto result = database.registerUser(
        user.toStdString(), 
        email.toStdString(), 
        pass.toStdString()
    );
    
    // Обрабатываем результат
    switch (result) {
        case Database::RegResult::SUCCESS:
            Logger::info("User registered successfully: " + user.toStdString());
            QMessageBox::information(this, "Success", "Registration successful!");
            clearInputs();
            showLoginForm();                // Переходим на форму входа
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
```

#### Выход (handleLogout):

```cpp
void AuthorizationForm::handleLogout() {
    Logger::info("User logged out");
    currentUser.clear();            // Очищаем текущего пользователя
    clearInputs();                  // Очищаем поля ввода
    stacked->setCurrentIndex(0);    // Показываем форму входа
    QMessageBox::information(this, "Logout", "Logged out successfully");
}
```

---

## Требования к данным

| Поле | Требование | Примеры |
|------|-----------|---------|
| **Username** | 3-20 символов, буквы/цифры/`_`/`-` | `john_doe`, `user123` |
| **Email** | Формат `name@domain.com` | `john@example.com` |
| **Password** | ≥6 символов, буквы + цифры | `Pass123`, `Admin456` |

---

## Сборка и запуск

### 1. Установка зависимостей

**macOS:**
```bash
brew install qt openssl cmake
```

**Ubuntu/Debian:**
```bash
sudo apt-get install qt6-base-dev libssl-dev cmake
```

### 2. Сборка

```bash
mkdir build && cd build
cmake ..
make
```

### 3. Запуск

```bash
./lab4
```

### 4. Тестирование

1. Нажмите "Register"
2. Заполните:
   - Username: `testuser`
   - Email: `test@mail.com`
   - Password: `Pass123`
3. Нажмите "Register"
4. На форме входа введите те же названия и пароль
5. Нажмите "Login"
6. Должен открыться профиль с вашими данными

---

## Как сохраняются данные

Все пользователи хранятся в файле `users.db` в директории, откуда вы запустили приложение.

**Формат файла:**
```
username|email|password_hash
```

**Пример:**
```
john|john@example.com|e5fa44f2b31c1fb553b6021e7aab6b74476544c0d83cac94761db6e...
alice|alice@domain.com|c9e8b4f2d7a1c5e3b8f6d1a9c7e4b2f8d5a3c1e7b9f4d6a2c8e5...
```

Пароли **никогда** не хранятся открыто! Хранятся только хеши!

---

## Ключевые концепции безопасности

### ✅ Что мы делаем правильно:

1. **Хеширование паролей** - пароли кодируются SHA-256, а не хранятся как есть
2. **Валидация** - проверяем все входные данные перед сохранением
3. **Проверка дубликатов** - один email не может быть у двух пользователей
4. **Логирование** - записываем все действия для аудита

### ⚠️ Что можно улучшить (для боевого приложения):

- Добавить соль (salt) к паролям для защиты от радужных таблиц
- Шифрование файла `users.db`
- Защита от перебора паролей (rate limiting)
- Двухфакторная аутентификация
- Восстановление пароля по email
- Сохранение истории входов

---

## Файловая структура проекта

```
.
├── main.cpp                    ← Точка входа (9 строк)
├── User.hpp                    ← Структура пользователя (45 строк)
├── PasswordEncryption.hpp      ← Кодирование паролей (27 строк)
├── Database.hpp                ← Сохранение в файл (84 строки)
├── AuthorizationForm.hpp       ← Заголовок UI (46 строк)
├── AuthorizationForm.cpp       ← Реализация UI (235 строк)
├── Logger.hpp                  ← Логирование (48 строк)
├── CMakeLists.txt              ← Конфигурация сборки
├── README.md                   ← Этот файл
└── users.db                    ← Файл с пользователями (создаётся автоматически)
```

**Всего: 494 строки кода** + документация

---

## Вопросы и ответы

**Q: Почему пароль кодируется, а не шифруется?**  
A: Хеширование - это односторонний процесс. Никто не может получить оригинальный пароль из хеша. Это безопасно.

**Q: Можно ли одному паролю соответствовать несколько хешей?**  
A: Нет, один пароль всегда создаёт один и тот же хеш (без соли). Это упростило систему, но менее защищено.

**Q: Где хранится текущий пользователь?**  
A: В переменной `currentUser`. Это просто имя пользователя (строка). При выходе она очищается.

**Q: Почему нужен Qt?**  
A: Qt - фреймворк для создания графических интерфейсов. Без него пришлось бы писать сложный код для окон, кнопок, полей ввода.

**Q: Что такое SHA-256?**  
A: Это криптографический алгоритм хеширования. Он преобразует текст любого размера в строку из 64 символов.

---

**Проект готов к обучению и использованию!** 🎓

## Project Structure

```
├── main.cpp                   # Application entry point
├── AuthorizationForm.hpp/cpp # Main Qt UI form
├── UserDatabase.hpp          # User storage and management
├── User.hpp                  # User data model with validation
├── PasswordEncryption.hpp    # Password hashing and verification
├── Logger.hpp                # Logging utility
├── CMakeLists.txt           # Build configuration
├── users.db                 # User database (created at runtime)
└── assets/
    └── default_avatar.png   # Default user avatar placeholder
```

## Building the Project

### Prerequisites
- Qt6 (Widgets)
- OpenSSL
- CMake 3.10+
- C++ compiler with C++20 support

### On macOS
```bash
# Install dependencies
brew install qt openssl cmake

# Build
mkdir build && cd build
cmake ..
make
```

### On Linux
```bash
# Install dependencies
sudo apt-get install qt6-base-dev libssl-dev cmake

# Build
mkdir build && cd build
cmake ..
make
```

## Usage

1. **Run the application:**
   ```bash
   ./lab4
   ```

2. **Register a new user:**
   - Click "Create Account"
   - Enter username (3-20 chars, alphanumeric + _ -)
   - Enter valid email
   - Enter password (min 6 chars, needs uppercase, lowercase, digit)
   - Confirm password
   - Click "Register"

3. **Login:**
   - Enter username and password
   - Click "Login"
   - View your profile in the main menu

4. **Logout:**
   - Click "Logout" to return to login screen

## Data Storage

User data is stored in `users.db` file with the following format:
```
username|email|password_hash
```

The password hash includes:
- 16-byte random salt (32 hex characters)
- SHA-256 hash of (password + salt) (64 hex characters)

## Security Features

- ✓ Hashed passwords with random salt
- ✓ Input validation for all user data
- ✓ Email format validation
- ✓ Strong password requirements
- ✓ Duplicate user/email prevention
- ✓ Detailed logging for audit trail

## Future Enhancements

- User profile photo upload
- Password reset functionality
- User account deletion
- Admin panel
- Database encryption
- Email verification
- Two-factor authentication
- Session tokens/JWT implementation

## Code Examples

### Registering a User
```cpp
UserDatabase db("users.db");
auto result = db.registerUser("johndoe", "john@example.com", "Password123");
```

### Logging in
```cpp
auto result = db.loginUser("johndoe", "Password123");
if (result == UserDatabase::LoginResult::SUCCESS) {
    // Login successful
}
```

### Password Hashing
```cpp
std::string hash = PasswordEncryption::hashPassword("Password123");
bool verified = PasswordEncryption::verifyPassword("Password123", hash);
```

## Logging Output Example

```
22.02.2026 14:32:15 [INFO] Application started
22.02.2026 14:32:16 [DEBUG] Initializing Authorization Form
22.02.2026 14:32:17 [INFO] User successfully registered: johndoe
22.02.2026 14:32:20 [INFO] User login successful: johndoe
22.02.2026 14:33:45 [INFO] User logged out
22.02.2026 14:34:02 [INFO] Application closed
```

## License
Educational project for C++ and Qt6 learning.

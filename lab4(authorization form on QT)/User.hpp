#pragma once

#include <string>
#include <cctype>

struct User {
    std::string username;
    std::string email;
    std::string passwordHash;

    User() = default;
    User(const std::string& u, const std::string& e, const std::string& h)
        : username(u), email(e), passwordHash(h) {}

    bool isValid() const {
        return !username.empty() && !email.empty() && !passwordHash.empty();
    }

    // Валидация имени пользователя (3-20 символов, буквы, цифры, _, -)
    static bool isValidUsername(const std::string& username) {
        if (username.length() < 3 || username.length() > 20) return false;
        for (char c : username) {
            if (!std::isalnum(c) && c != '_' && c != '-') return false;
        }
        return true;
    }

    // Валидация email
    static bool isValidEmail(const std::string& email) {
        size_t at = email.find('@');
        size_t dot = email.rfind('.');
        return at != std::string::npos && dot != std::string::npos && at < dot;
    }

    // Валидация пароля (минимум 6 символов, буквы и цифры)
    static bool isValidPassword(const std::string& password) {
        if (password.length() < 6) return false;
        bool hasLetter = false, hasDigit = false;
        for (char c : password) {
            if (std::isalpha(c)) hasLetter = true;
            if (std::isdigit(c)) hasDigit = true;
        }
        return hasLetter && hasDigit;
    }
};

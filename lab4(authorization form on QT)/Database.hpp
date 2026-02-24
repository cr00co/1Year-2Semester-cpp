#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <cctype>
#include "User.hpp"
#include "PasswordEncryption.hpp"

class Database {
private:
    std::vector<User> users;
    std::string filePath;

    void loadFromFile() {
        users.clear();
        std::ifstream file(filePath);
        if (!file) return;

        std::string line;
        while (std::getline(file, line)) {
            size_t pos = line.find('|');
            if (pos != std::string::npos) {
                users.emplace_back(
                    line.substr(0, pos),
                    line.substr(pos + 1)
                );
            }
        }
    }

    void saveToFile() const {
        std::ofstream file(filePath);
        for (const auto& u : users) {
            file << u.username << "|" << u.passwordHash << "\n";
        }
    }

    bool isValidUsername(const std::string& u) const {
        return u.length() >= 3 && u.length() <= 20;
    }



    bool isValidPassword(const std::string& p) const {
        if (p.length() < 6) return false;
        bool hasLetter = false, hasDigit = false;
        for (char c : p) {
            if (std::isalpha(c)) hasLetter = true;
            if (std::isdigit(c)) hasDigit = true;
        }
        return hasLetter && hasDigit;
    }

public:
    enum class RegResult { SUCCESS, USERNAME_EXISTS, INVALID_DATA };

    Database(const std::string& path = "users.db") : filePath(path) {
        loadFromFile();
    }

    RegResult registerUser(const std::string& username, const std::string& password) {
        if (!isValidUsername(username) || !isValidPassword(password)) {
            return RegResult::INVALID_DATA;
        }

        for (const auto& u : users) {
            if (u.username == username) return RegResult::USERNAME_EXISTS;
        }

        users.emplace_back(username, PasswordEncryption::hashPassword(password));
        saveToFile();
        return RegResult::SUCCESS;
    }

    bool loginUser(const std::string& username, const std::string& password) {
        for (const auto& u : users) {
            if (u.username == username && PasswordEncryption::verifyPassword(password, u.passwordHash)) {
                return true;
            }
        }
        return false;
    }

    const User* getUser(const std::string& username) const {
        for (const auto& u : users) {
            if (u.username == username) return &u;
        }
        return nullptr;
    }
};

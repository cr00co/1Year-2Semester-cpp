#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
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
            if (line.empty()) continue;
            std::stringstream ss(line);
            std::string username, email, hash;
            if (std::getline(ss, username, '|') && 
                std::getline(ss, email, '|') && 
                std::getline(ss, hash, '|')) {
                users.emplace_back(username, email, hash);
            }
        }
        file.close();
    }

    bool saveToFile() {
        std::ofstream file(filePath);
        if (!file) return false;
        for (const auto& u : users) {
            file << u.username << "|" << u.email << "|" << u.passwordHash << "\n";
        }
        file.close();
        return true;
    }

public:
    Database(const std::string& path = "users.db") : filePath(path) {
        loadFromFile();
    }

    enum class RegResult { SUCCESS, USERNAME_EXISTS, EMAIL_EXISTS, INVALID_USERNAME, INVALID_EMAIL, INVALID_PASSWORD };

    RegResult registerUser(const std::string& username, const std::string& email, const std::string& password) {
        if (!User::isValidUsername(username)) return RegResult::INVALID_USERNAME;
        if (!User::isValidEmail(email)) return RegResult::INVALID_EMAIL;
        if (!User::isValidPassword(password)) return RegResult::INVALID_PASSWORD;

        for (const auto& u : users) {
            if (u.username == username) return RegResult::USERNAME_EXISTS;
            if (u.email == email) return RegResult::EMAIL_EXISTS;
        }

        std::string hash = PasswordEncryption::hashPassword(password);
        if (hash.empty()) return RegResult::SUCCESS;

        users.emplace_back(username, email, hash);
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

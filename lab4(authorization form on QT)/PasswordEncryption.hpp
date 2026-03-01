#pragma once

#include <string>
#include <openssl/sha.h>
#include <sstream>
#include <iomanip>

class PasswordEncryption {
public:
    // Хеширование пароля (SHA-256)
    static std::string hashPassword(const std::string& password) {
        unsigned char hash[SHA256_DIGEST_LENGTH];
        SHA256(reinterpret_cast<unsigned char*>(const_cast<char*>(password.data())), 
               password.length(), hash);

        std::stringstream ss;
        for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
            ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];

        return ss.str();
    }

    static bool verifyPassword(const std::string& password, const std::string& hash) {
        return hashPassword(password) == hash;
    }
};

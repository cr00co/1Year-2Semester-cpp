#pragma once
#include <string>

struct User {
    std::string username;
    std::string passwordHash;

    User(const std::string& u, const std::string& h)
        : username(u), passwordHash(h) {}
};

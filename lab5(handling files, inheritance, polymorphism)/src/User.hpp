#pragma once
#include <string>
#include <QString>
#include <QStringList>

struct User {
    QString     username;
    QString     passwordHash;
    QString     avatar;        // path to avatar, empty = placeholder
    QStringList photos;        // user photo paths (max 4)

    static constexpr int MAX_PHOTOS = 4;

    User() = default;
    User(const QString& u, const QString& h) : username(u), passwordHash(h) {}
};

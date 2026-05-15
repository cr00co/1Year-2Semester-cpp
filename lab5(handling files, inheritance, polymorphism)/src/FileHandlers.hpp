#pragma once
#include "User.hpp"
#include "PasswordEncryption.hpp"
#include "Logger.hpp"
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QFileInfo>
#include <QUuid>
#include <QList>

// -------------------------------------------------------------
// FileHandler - abstract base class (lab requirement: inheritance)
// -------------------------------------------------------------
class FileHandler {
public:
    explicit FileHandler(const QString& path) : m_path(path) {}
    virtual ~FileHandler() = default;
    virtual bool load() = 0;
    virtual bool save() = 0;
protected:
    QString m_path;
};

// -------------------------------------------------------------
// UserFileHandler - reads and writes users.csv
// Format: username|passwordHash|avatar|photo1;photo2;photo3
// -------------------------------------------------------------
class UserFileHandler : public FileHandler {
public:
    explicit UserFileHandler(const QString& path) : FileHandler(path) {}

    bool load() {
        m_users.clear();
        QFile file(m_path);
        if (!file.exists()) return true;
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return false;
        QTextStream in(&file);
        while (!in.atEnd()) {
            QStringList p = in.readLine().split('|');
            if (p.size() < 2) continue;
            User u(p[0], p[1]);
            if (p.size() > 2) u.avatar = p[2];
            if (p.size() > 3 && !p[3].isEmpty())
                u.photos = p[3].split(';', Qt::SkipEmptyParts);
            m_users.append(u);
        }
        return true;
    }

    bool save() override {
        QDir().mkpath(QFileInfo(m_path).absolutePath());
        QFile file(m_path);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
            return false;
        QTextStream out(&file);
        for (const User& u : m_users)
            out << u.username << '|' << u.passwordHash << '|'
                << u.avatar << '|' << u.photos.join(';') << '\n';
        return true;
    }

    User* find(const QString& username) {
        for (User& u : m_users)
            if (u.username == username) return &u;
        return nullptr;
    }

    bool exists(const QString& username) { return find(username) != nullptr; }

    bool authenticate(const QString& username, const QString& password) {
        User* u = find(username);
        return u && PasswordEncryption::verifyPassword(
            password.toStdString(), u->passwordHash.toStdString());
    }

    // Returns error text, or "" on success (same pattern as lab4)
    QString registerUser(const QString& username, const QString& password) {
        if (username.length() < 3 || username.length() > 20)
            return "Login must be 3-20 characters.";
        if (password.length() < 6)
            return "Password must be at least 6 characters.";
        bool hasLetter = false, hasDigit = false;
        for (QChar c : password) {
            if (c.isLetter()) hasLetter = true;
            if (c.isDigit())  hasDigit  = true;
        }
        if (!hasLetter || !hasDigit)
            return "Password must contain letters and digits.";
        if (exists(username))
            return "Username already taken.";
        m_users.append(User(username,
            QString::fromStdString(PasswordEncryption::hashPassword(password.toStdString()))));
        save();
        Logger::info("Registered: " + username.toStdString());
        return "";
    }

    QList<User>& users() { return m_users; }

private:
    QList<User> m_users; // Encapsulation
};

// -------------------------------------------------------------
// PhotoFileHandler - copies and deletes files in photos/ folder
// -------------------------------------------------------------
class PhotoFileHandler : public FileHandler { // Inheritance
public:
    explicit PhotoFileHandler(const QString& dir) : FileHandler(dir) {}

    bool load() override { QDir().mkpath(m_path); return true; }
    bool save() override { return true; }

    // Copies srcPath into photos/, returns new path or ""
    QString copyPhoto(const QString& srcPath, const QString& username) {
        QDir().mkpath(m_path);
        QString ext  = QFileInfo(srcPath).suffix().toLower();
        QString dest = m_path + "/" + username + "_"
                       + QUuid::createUuid().toString(QUuid::Id128).left(8) + "." + ext;
        if (!QFile::copy(srcPath, dest)) {
            Logger::error("Failed to copy: " + srcPath.toStdString());
            return "";
        }
        return dest;
    }

    bool deletePhoto(const QString& path) {
        if (!QFile::remove(path)) {
            Logger::error("Failed to delete: " + path.toStdString());
            return false;
        }
        return true;
    }
};

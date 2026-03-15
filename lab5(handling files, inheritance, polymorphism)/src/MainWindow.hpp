#pragma once
#include <QWidget>
#include <QLabel>
#include <QTimer>
#include <memory>
#include "FileHandlers.hpp"
#include "PhotoOperations.hpp"

class MainWindow : public QWidget {
    Q_OBJECT
public:
    MainWindow(User* user, UserFileHandler* ufh, PhotoFileHandler* pfh,
               QWidget* parent = nullptr);
signals:
    void loggedOut();

private slots:
    void onUpload();
    void onChoose();

private:
    void refreshAvatar();
    void setStatus(const QString& msg, bool ok);

    // Draws a simple person SVG as avatar placeholder
    QPixmap makePlaceholder(int size);

    User*             m_user;
    UserFileHandler*  m_ufh;
    PhotoFileHandler* m_pfh;

    // Polymorphic operations
    std::unique_ptr<PhotoOperation> m_upload;
    std::unique_ptr<PhotoOperation> m_choose;

    QLabel*  m_avatarLabel;
    QLabel*  m_statusLabel;
    QTimer*  m_statusTimer;   // clears status after 10 seconds
};

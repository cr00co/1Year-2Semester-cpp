#pragma once
#include <QDialog>
#include <QListWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QPixmap>
#include <QFileInfo>
#include <functional>
#include <memory>
#include "FileHandlers.hpp"
#include "PhotoOperations.hpp"

class PhotoChooserWindow : public QDialog {
public:
    // onChanged is called whenever photos or avatar are modified
    PhotoChooserWindow(User* user, UserFileHandler* ufh, PhotoFileHandler* pfh,
                       std::function<void()> onChanged,
                       QWidget* parent = nullptr)
        : QDialog(parent), m_user(user), m_onChanged(onChanged)
    {
        // Polymorphic operations
        m_delete    = std::make_unique<DeletePhoto>(pfh, ufh);
        m_setAvatar = std::make_unique<SetAvatarPhoto>(ufh);

        setWindowTitle("Choose Photo");
        setMinimumSize(400, 320);

        auto* root = new QVBoxLayout(this);

        // Thumbnail grid
        m_list = new QListWidget;
        m_list->setViewMode(QListView::IconMode);
        m_list->setIconSize({80, 80});
        m_list->setGridSize({100, 110});
        m_list->setResizeMode(QListView::Adjust);
        m_list->setMovement(QListView::Static);
        root->addWidget(m_list);

        // Status label
        m_statusLabel = new QLabel;
        m_statusLabel->setWordWrap(true);
        root->addWidget(m_statusLabel);

        // Buttons matching the diagram
        auto* btnRow    = new QHBoxLayout;
        auto* setAvBtn  = new QPushButton("set avatar");
        auto* deleteBtn = new QPushButton("delete");
        btnRow->addStretch();
        btnRow->addWidget(setAvBtn);
        btnRow->addWidget(deleteBtn);
        root->addLayout(btnRow);

        connect(setAvBtn,  &QPushButton::clicked, this, [this]{ onSetAvatar(); });
        connect(deleteBtn, &QPushButton::clicked, this, [this]{ onDelete(); });

        refreshList();
    }

private:
    void onSetAvatar() {
        QString path = selectedPath();
        if (path.isEmpty()) { setStatus("Select a photo first.", false); return; }
        m_setAvatar->execute(m_user, path);
        setStatus(m_setAvatar->message(), m_setAvatar->ok());
        if (m_setAvatar->ok()) m_onChanged();
    }

    void onDelete() {
        QString path = selectedPath();
        if (path.isEmpty()) { setStatus("Select a photo first.", false); return; }
        m_delete->execute(m_user, path);
        setStatus(m_delete->message(), m_delete->ok());
        if (m_delete->ok()) { refreshList(); m_onChanged(); }
    }

    void refreshList() {
        m_list->clear();
        for (const QString& path : m_user->photos) {
            QPixmap px(path);
            auto* item = new QListWidgetItem(
                QIcon(px.scaled(80, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation)),
                QFileInfo(path).fileName() + (path == m_user->avatar ? " *" : ""));
            item->setData(Qt::UserRole, path);
            m_list->addItem(item);
        }
        if (m_user->photos.isEmpty())
            m_list->addItem("No photos yet.");
    }

    QString selectedPath() const {
        auto* item = m_list->currentItem();
        return item ? item->data(Qt::UserRole).toString() : "";
    }

    void setStatus(const QString& msg, bool ok) {
        m_statusLabel->setStyleSheet(ok ? "color:green;" : "color:red;");
        m_statusLabel->setText(msg);
    }

    User*                 m_user;
    std::function<void()> m_onChanged;
    QListWidget*          m_list;
    QLabel*               m_statusLabel;
    std::unique_ptr<PhotoOperation> m_delete;
    std::unique_ptr<PhotoOperation> m_setAvatar;
};

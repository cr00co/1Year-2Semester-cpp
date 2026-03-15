#pragma once
#include "User.hpp"
#include "FileHandlers.hpp"
#include <QDialog>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPixmap>
#include <QFileInfo>
#include <QImageReader>
#include <QWidget>

// -------------------------------------------------------------
// PhotoOperation - abstract base class (lab requirement: polymorphism)
// MainWindow stores pointers of this type and calls execute()
// -------------------------------------------------------------
class PhotoOperation {
public:
    virtual ~PhotoOperation() = default;
    virtual void execute(User* user, const QString& photoPath = "") = 0;

    bool    ok()      const { return m_ok; }
    QString message() const { return m_message; }

protected:
    void setResult(bool ok, const QString& msg) { m_ok = ok; m_message = msg; }

private:
    bool    m_ok = false;
    QString m_message;
};

// -------------------------------------------------------------
// UploadPhoto
// -------------------------------------------------------------
class UploadPhoto : public PhotoOperation {
public:
    UploadPhoto(PhotoFileHandler* pfh, UserFileHandler* ufh) : m_pfh(pfh), m_ufh(ufh) {}

    void execute(User* user, const QString& photoPath = "") override {
        if (!user) { setResult(false, "No user."); return; }
        if (user->photos.size() >= User::MAX_PHOTOS) {
            setResult(false, QString("Max %1 photos allowed.").arg(User::MAX_PHOTOS));
            return;
        }
        if (!QImageReader(photoPath).canRead()) {
            setResult(false, "Not a valid image.");
            return;
        }
        QString dest = m_pfh->copyPhoto(photoPath, user->username);
        if (dest.isEmpty()) { setResult(false, "Failed to copy file."); return; }

        user->photos.append(dest);
        m_ufh->save();
        Logger::info("Photo uploaded for " + user->username.toStdString());
        setResult(true, "Photo uploaded.");
    }
private:
    PhotoFileHandler* m_pfh;
    UserFileHandler*  m_ufh;
};

// -------------------------------------------------------------
// DeletePhoto
// -------------------------------------------------------------
class DeletePhoto : public PhotoOperation {
public:
    DeletePhoto(PhotoFileHandler* pfh, UserFileHandler* ufh) : m_pfh(pfh), m_ufh(ufh) {}

    void execute(User* user, const QString& photoPath = "") override {
        if (!user || photoPath.isEmpty()) { setResult(false, "No photo selected."); return; }
        if (!m_pfh->deletePhoto(photoPath)) { setResult(false, "Failed to delete."); return; }

        user->photos.removeOne(photoPath);
        if (user->avatar == photoPath)
            user->avatar = ""; // avatar deleted - reset to placeholder
        m_ufh->save();
        Logger::info("Photo deleted: " + photoPath.toStdString());
        setResult(true, "Photo deleted.");
    }
private:
    PhotoFileHandler* m_pfh;
    UserFileHandler*  m_ufh;
};

// -------------------------------------------------------------
// SetAvatarPhoto
// -------------------------------------------------------------
class SetAvatarPhoto : public PhotoOperation {
public:
    explicit SetAvatarPhoto(UserFileHandler* ufh) : m_ufh(ufh) {}

    void execute(User* user, const QString& photoPath = "") override {
        if (!user || photoPath.isEmpty()) { setResult(false, "No photo selected."); return; }
        user->avatar = photoPath;
        m_ufh->save();
        Logger::info("Avatar set for " + user->username.toStdString());
        setResult(true, "Avatar updated.");
    }
private:
    UserFileHandler* m_ufh;
};

// -------------------------------------------------------------
// ChoosePhoto - opens photo chooser dialog
// -------------------------------------------------------------

// PhotoDialog - embedded here to avoid extra files
class PhotoDialog : public QDialog {
public:
    explicit PhotoDialog(const QStringList& photos, QWidget* parent = nullptr)
        : QDialog(parent)
    {
        setWindowTitle("Choose Photo");
        setMinimumSize(400, 300);
        auto* layout = new QVBoxLayout(this);

        m_list = new QListWidget;
        m_list->setViewMode(QListView::IconMode);
        m_list->setIconSize({80, 80});
        m_list->setGridSize({100, 110});
        m_list->setResizeMode(QListView::Adjust);
        m_list->setMovement(QListView::Static);

        for (const QString& path : photos) {
            QPixmap px(path);
            auto* item = new QListWidgetItem(
                QIcon(px.scaled(80, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation)),
                QFileInfo(path).fileName());
            item->setData(Qt::UserRole, path);
            m_list->addItem(item);
        }
        if (photos.isEmpty())
            m_list->addItem("No photos yet.");

        auto* okBtn     = new QPushButton("Select");
        auto* cancelBtn = new QPushButton("Cancel");
        okBtn->setEnabled(false);

        auto* btnRow = new QHBoxLayout;
        btnRow->addStretch();
        btnRow->addWidget(okBtn);
        btnRow->addWidget(cancelBtn);

        layout->addWidget(m_list);
        layout->addLayout(btnRow);

        connect(m_list, &QListWidget::itemClicked, this, [okBtn](QListWidgetItem* item) {
            okBtn->setEnabled(item && item->data(Qt::UserRole).isValid());
        });
        connect(m_list,    &QListWidget::itemDoubleClicked, this, &QDialog::accept);
        connect(okBtn,     &QPushButton::clicked,           this, &QDialog::accept);
        connect(cancelBtn, &QPushButton::clicked,           this, &QDialog::reject);
    }

    QString selected() const {
        auto* item = m_list->currentItem();
        return item ? item->data(Qt::UserRole).toString() : "";
    }
private:
    QListWidget* m_list;
};

class ChoosePhoto : public PhotoOperation {
public:
    explicit ChoosePhoto(QWidget* parent = nullptr) : m_parent(parent) {}

    void execute(User* user, const QString& = "") override {
        m_chosen.clear();
        if (!user) { setResult(false, "No user."); return; }
        PhotoDialog dlg(user->photos, m_parent);
        if (dlg.exec() == QDialog::Accepted) {
            m_chosen = dlg.selected();
            setResult(true, "Photo selected.");
        } else {
            setResult(false, "Cancelled.");
        }
    }

    QString chosen() const { return m_chosen; }
private:
    QWidget* m_parent;
    QString  m_chosen;
};

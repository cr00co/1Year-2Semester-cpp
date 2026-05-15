#include "MainWindow.hpp"
#include "PhotoChooserWindow.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QFileDialog>
#include <QPixmap>
#include <QFileInfo>
#include <QPainter>

MainWindow::MainWindow(User* user, UserFileHandler* ufh, PhotoFileHandler* pfh,
                       QWidget* parent)
    : QWidget(parent), m_user(user), m_ufh(ufh), m_pfh(pfh)
{
    m_upload = std::make_unique<UploadPhoto>(pfh, ufh);
    m_choose = std::make_unique<ChoosePhoto>(this);

    // Auto-clear status label after 10 seconds
    m_statusTimer = new QTimer(this);
    m_statusTimer->setSingleShot(true);
    connect(m_statusTimer, &QTimer::timeout, this, [this]{ m_statusLabel->clear(); });

    setMinimumSize(500, 400);

    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(20, 20, 20, 20);
    root->setSpacing(12);

    // ── Top: welcome ──────────────────────────────────────────
    auto* welcomeLabel = new QLabel("Welcome, <b>" + user->username + "</b>!");
    welcomeLabel->setStyleSheet("font-size: 15px;");
    root->addWidget(welcomeLabel);

    // ── Center row: avatar square | buttons ───────────────────
    auto* centerRow = new QHBoxLayout;
    centerRow->setSpacing(24);

    // Avatar — square, fixed 140x140
    m_avatarLabel = new QLabel;
    m_avatarLabel->setFixedSize(140, 140);
    m_avatarLabel->setAlignment(Qt::AlignCenter);
    m_avatarLabel->setStyleSheet(
        "border: 2px solid #bdbdbd;"
        "background: #f5f5f5;");
    m_avatarLabel->setScaledContents(true);
    centerRow->addWidget(m_avatarLabel);

    // Right side: hint + buttons
    auto* rightCol = new QVBoxLayout;
    rightCol->setSpacing(8);

    // Avatar hint
    auto* hintLabel = new QLabel(
        "<b>Avatar requirements:</b><br>"
        "Format: JPG, PNG, BMP, GIF, WebP<br>"
        "Recommended size: 200×200 px or larger<br>"
        "Max photos per user: 4");
    hintLabel->setStyleSheet(
        "font-size: 11px; color: #757575;"
        "background: #fafafa; border: 1px solid #e0e0e0;"
        "border-radius: 4px; padding: 6px;");
    hintLabel->setWordWrap(true);
    rightCol->addWidget(hintLabel);

    rightCol->addSpacing(6);

    // Upload and Choose buttons
    QString btnStyle =
        "QPushButton {"
        "  font-size: 13px; padding: 7px 0px;"
        "  border-radius: 5px;"
        "  background: #1976D2; color: white; border: none;"
        "}"
        "QPushButton:hover   { background: #1565C0; }"
        "QPushButton:pressed { background: #0D47A1; }";

    auto* uploadBtn = new QPushButton("upload");
    auto* chooseBtn = new QPushButton("choose");
    uploadBtn->setStyleSheet(btnStyle);
    chooseBtn->setStyleSheet(btnStyle);

    rightCol->addWidget(uploadBtn);
    rightCol->addWidget(chooseBtn);
    rightCol->addStretch();

    centerRow->addLayout(rightCol);
    root->addLayout(centerRow);

    // ── Status label (auto-clears after 10s) ──────────────────
    m_statusLabel = new QLabel;
    m_statusLabel->setWordWrap(true);
    m_statusLabel->setMinimumHeight(18);
    m_statusLabel->setStyleSheet("font-size: 12px;");
    root->addWidget(m_statusLabel);

    root->addStretch();

    // ── Logout ────────────────────────────────────────────────
    auto* logoutBtn = new QPushButton("Logout");
    logoutBtn->setStyleSheet(
        "QPushButton {"
        "  font-size: 12px; padding: 5px 14px;"
        "  border-radius: 4px; border: 1px solid #bdbdbd;"
        "  background: #eeeeee; color: #333;"
        "}"
        "QPushButton:hover { background: #e0e0e0; }");
    logoutBtn->setFixedWidth(90);
    root->addWidget(logoutBtn, 0, Qt::AlignRight);

    connect(uploadBtn, &QPushButton::clicked, this, &MainWindow::onUpload);
    connect(chooseBtn, &QPushButton::clicked, this, &MainWindow::onChoose);
    connect(logoutBtn, &QPushButton::clicked, this, &MainWindow::loggedOut);

    refreshAvatar();
}

void MainWindow::onUpload() {
    QString path = QFileDialog::getOpenFileName(this, "Select Image", "",
        "Images (*.jpg *.jpeg *.png *.bmp *.gif *.webp)");
    if (path.isEmpty()) return;

    m_upload->execute(m_user, path);
    setStatus(m_upload->message(), m_upload->ok());
    if (m_upload->ok()) refreshAvatar();
}

void MainWindow::onChoose() {
    PhotoChooserWindow w(m_user, m_ufh, m_pfh, [this]{ refreshAvatar(); }, this);
    w.exec();
}

void MainWindow::refreshAvatar() {
    if (!m_user->avatar.isEmpty() && QFileInfo::exists(m_user->avatar)) {
        m_avatarLabel->setPixmap(QPixmap(m_user->avatar));
    } else {
        m_avatarLabel->setPixmap(makePlaceholder(140));
    }
}

QPixmap MainWindow::makePlaceholder(int size) {
    // Simple person silhouette
    QPixmap px(size, size);
    px.fill(QColor("#f5f5f5"));
    QPainter p(&px);
    p.setRenderHint(QPainter::Antialiasing);
    p.setBrush(QColor("#bdbdbd"));
    p.setPen(Qt::NoPen);

    double s = size;
    // Head
    double headR = s * 0.18;
    p.drawEllipse(QRectF(s/2 - headR, s * 0.15, headR * 2, headR * 2));
    // Body
    double bw = s * 0.40, bh = s * 0.28;
    p.drawRoundedRect(QRectF(s/2 - bw/2, s * 0.15 + headR*2 + s*0.04, bw, bh), 10, 10);

    return px;
}

void MainWindow::setStatus(const QString& msg, bool ok) {
    m_statusLabel->setStyleSheet(ok
        ? "font-size:12px; color: #2e7d32;"
        : "font-size:12px; color: #c62828;");
    m_statusLabel->setText(msg);
    m_statusTimer->start(10000);
}

#include "MainWindow.h"

#include <QApplication>
#include <QScreen>
#include <QFrame>
#include <QFont>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_fetcher(new QuoteFetcher(this))
{
    setWindowTitle("Рандомные цитаты");
    setMinimumSize(600, 420);
    resize(700, 480);

    if (QScreen* screen = QApplication::primaryScreen()) {
        const QRect sg = screen->availableGeometry();
        move(sg.center() - rect().center());
    }

    buildUi();
    applyStyles();

    connect(m_fetcher, &QuoteFetcher::quoteReady,    this, &MainWindow::onQuoteReady);
    connect(m_fetcher, &QuoteFetcher::errorOccurred, this, &MainWindow::onError);
    connect(m_fetchBtn, &QPushButton::clicked,        this, &MainWindow::onFetchClicked);
}


void MainWindow::buildUi() {
    QWidget* central = new QWidget(this);
    setCentralWidget(central);

    QVBoxLayout* root = new QVBoxLayout(central);
    root->setContentsMargins(48, 36, 48, 32);
    root->setSpacing(0);

    QLabel* title = new QLabel("Рандомные цитаты", this);
    title->setObjectName("title");
    title->setAlignment(Qt::AlignCenter);
    root->addWidget(title);

    root->addSpacing(132);

    m_quoteLabel = new QLabel("Нажмите кнопку, чтобы получить цитату", this);
    m_quoteLabel->setObjectName("quoteLabel");
    m_quoteLabel->setWordWrap(true);
    m_quoteLabel->setAlignment(Qt::AlignCenter);
    root->addWidget(m_quoteLabel, 1);

    m_authorLabel = new QLabel("", this);
    m_authorLabel->setObjectName("authorLabel");
    m_authorLabel->setAlignment(Qt::AlignCenter);
    root->addWidget(m_authorLabel);

    root->addSpacing(115);

    m_progressBar = new QProgressBar(this);
    m_progressBar->setObjectName("progressBar");
    m_progressBar->setRange(0, 0);
    m_progressBar->setFixedHeight(3);
    m_progressBar->setTextVisible(false);
    m_progressBar->hide();
    root->addWidget(m_progressBar);

    root->addSpacing(10);

    m_fetchBtn = new QPushButton("Получить цитату", this);
    m_fetchBtn->setObjectName("fetchBtn");
    m_fetchBtn->setFixedHeight(50);
    m_fetchBtn->setCursor(Qt::PointingHandCursor);
    root->addWidget(m_fetchBtn);
}


void MainWindow::applyStyles() {
    setStyleSheet(R"(
        QMainWindow, QWidget {
            background-color: #0f1117;
            color: #e2e8f0;
        }

        QLabel#title {
            font-size: 24px;
            font-weight: 700;
            color: #f1f5f9;
            letter-spacing: 0.5px;
        }

        QLabel#quoteLabel {
            font-size: 18px;
            color: #cbd5e1;
            padding: 0 16px;
        }

        QLabel#authorLabel {
            font-size: 14px;
            color: #f8fafc;
            padding: 0 16px;
            font-style: italic;
        }

        QPushButton#fetchBtn {
            background-color: #6366f1;
            color: #ffffff;
            border: none;
            border-radius: 12px;
            font-size: 15px;
            font-weight: 600;
        }

        QPushButton#fetchBtn:hover {
            background-color: #818cf8;
        }

        QPushButton#fetchBtn:pressed {
            background-color: #4f46e5;
        }

        QPushButton#fetchBtn:disabled {
            background-color: #1e293b;
            color: #475569;
        }

        QProgressBar#progressBar {
            background-color: #1e293b;
            border: none;
            border-radius: 2px;
        }

        QProgressBar#progressBar::chunk {
            background-color: #6366f1;
            border-radius: 2px;
        }
    )");

    QFont appFont("Segoe UI", 11);
    appFont.setStyleStrategy(QFont::PreferAntialias);
    QApplication::setFont(appFont);
}


void MainWindow::onFetchClicked() {
    setLoading(true);
    m_quoteLabel->setText("Загружаем…");
    m_fetcher->fetchQuotes();
}

void MainWindow::onQuoteReady(const Quote& quote) {
    setLoading(false);
    m_quoteLabel->setText("«" + quote.text + "»");
    m_authorLabel->setText("— " + quote.author);
}

void MainWindow::onError(const QString& message) {
    setLoading(false);
    m_quoteLabel->setText("⚠️  " + message);
    m_authorLabel->setText("");
}

void MainWindow::setLoading(bool loading) {
    m_fetchBtn->setEnabled(!loading);
    m_progressBar->setVisible(loading);
}

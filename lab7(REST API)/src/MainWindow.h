#pragma once

#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QProgressBar>

#include "QuoteFetcher.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

private slots:
    void onFetchClicked();
    void onQuoteReady(const Quote& quote);
    void onError(const QString& message);

private:
    void buildUi();
    void applyStyles();
    void setLoading(bool loading);

    QuoteFetcher* m_fetcher;

    QLabel*       m_quoteLabel;
    QLabel*       m_authorLabel;
    QPushButton*  m_fetchBtn;
    QProgressBar* m_progressBar;
};

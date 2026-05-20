#pragma once

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QList>
#include <QString>
#include <QStringList>

struct Quote {
    QString text;
    QString author;
    QString tags;
};

class QuoteFetcher : public QObject {
    Q_OBJECT

public:
    explicit QuoteFetcher(QObject* parent = nullptr);
    void fetchQuotes();

signals:
    void quoteReady(const Quote& quote);
    void errorOccurred(const QString& message);

private slots:
    void onReplyFinished(QNetworkReply* reply);

private:
    QNetworkAccessManager* m_manager;
    QList<Quote> m_quotes;

    QList<Quote> parseQuotesFromHtml(const QString& html);
    Quote pickRandomQuote();
};

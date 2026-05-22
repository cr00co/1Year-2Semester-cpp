#pragma once

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QString>
#include <QStringList>

struct Quote {
    QString text;
    QString author;
    QString tags;
};

class QuoteClient : public QObject {
    Q_OBJECT

public:
    explicit QuoteClient(QObject* parent = nullptr);
    void fetchRandomQuote();

signals:
    void quoteReceived(const Quote& quote);
    void errorOccurred(const QString& message);

private slots:
    void onReplyFinished(QNetworkReply* reply);

private:
    enum class Source {
        QuotesApi,
        TranslationApi
    };

    void fetchQuotes();
    void translatePendingQuote();
    void sendRequest(const QUrl& url);
    void emitQuote(const QString& text, const QString& author, const QStringList& tags);

    QNetworkAccessManager* m_manager;
    Source m_currentSource;
    QString m_pendingText;
    QString m_pendingAuthor;
    QStringList m_pendingTags;
};

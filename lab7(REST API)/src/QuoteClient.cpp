#include "QuoteClient.h"

#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QUrl>
#include <QUrlQuery>

static const QString API_URL = "https://api.forismatic.com/api/1.0/";

QuoteClient::QuoteClient(QObject* parent)
    : QObject(parent)
    , m_manager(new QNetworkAccessManager(this))
    , m_retryCount(0)
{
    connect(m_manager, &QNetworkAccessManager::finished,
            this, &QuoteClient::onReplyFinished);
}

void QuoteClient::fetchRandomQuote() {
    QUrl url(API_URL);

    QUrlQuery query;
    query.addQueryItem("method", "getQuote");
    query.addQueryItem("format", "json");
    query.addQueryItem("lang",   "ru");

    url.setQuery(query);

    QNetworkRequest request(url);
    m_manager->get(request);
}

void QuoteClient::onReplyFinished(QNetworkReply* reply) {
    reply->deleteLater();

    if (reply->error() != QNetworkReply::NoError) {
        emit errorOccurred("Сетевая ошибка: " + reply->errorString());
        return;
    }

    const QByteArray data = reply->readAll();
    const QJsonDocument doc = QJsonDocument::fromJson(data);

    if (doc.isNull() || !doc.isObject()) {
        emit errorOccurred("Ошибка разбора ответа сервера");
        return;
    }

    const QJsonObject obj = doc.object();
    const QString text   = obj["quoteText"].toString().trimmed();
    const QString author = obj["quoteAuthor"].toString().trimmed();

    if (text.isEmpty()) {
        emit errorOccurred("Сервер вернул пустую цитату");
        return;
    }

    if (author.isEmpty()) {
        if (m_retryCount < MAX_RETRIES) {
            m_retryCount++;
            fetchRandomQuote();
            return;
        } else {
            emit errorOccurred("Не удалось найти цитату с известным автором.");
            m_retryCount = 0;
            return;
        }
    }

    m_retryCount = 0;
    Quote quote;
    quote.text   = text;
    quote.author = author;

    emit quoteReceived(quote);
}
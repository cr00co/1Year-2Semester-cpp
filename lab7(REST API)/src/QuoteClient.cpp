#include "QuoteClient.h"

#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUrl>
#include <QUrlQuery>
#include <QRandomGenerator>

static const QString QUOTES_API_URL = "https://quotes.toscrape.com/api/quotes";
static const QString TRANSLATION_API_URL = "https://api.mymemory.translated.net/get";
static constexpr int QUOTES_PAGE_COUNT = 10;

QuoteClient::QuoteClient(QObject* parent)
    : QObject(parent)
    , m_manager(new QNetworkAccessManager(this))
    , m_currentSource(Source::QuotesApi)
{
    connect(m_manager, &QNetworkAccessManager::finished,
            this, &QuoteClient::onReplyFinished);
}

void QuoteClient::fetchRandomQuote() {
    fetchQuotes();
}

void QuoteClient::fetchQuotes() {
    m_currentSource = Source::QuotesApi;

    QUrl url(QUOTES_API_URL);
    QUrlQuery query;
    const int page = QRandomGenerator::global()->bounded(QUOTES_PAGE_COUNT) + 1;
    query.addQueryItem("page", QString::number(page));
    url.setQuery(query);

    sendRequest(url);
}

void QuoteClient::translatePendingQuote() {
    m_currentSource = Source::TranslationApi;
    
    if (m_pendingText.size() > 500) {
        m_pendingText = m_pendingText.left(500);
        const int lastSpace = m_pendingText.lastIndexOf(' ');
        if (lastSpace > 0)
            m_pendingText = m_pendingText.left(lastSpace);
        m_pendingText += "…";
    }

    QUrl url(TRANSLATION_API_URL);
    QUrlQuery query;
    query.addQueryItem("q", m_pendingText);
    query.addQueryItem("langpair", "en|ru");
    url.setQuery(query);

    sendRequest(url);
}

void QuoteClient::sendRequest(const QUrl& url) {
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::UserAgentHeader,
                      "Mozilla/5.0 (compatible; QtQuoteApp/1.0)");
    request.setRawHeader("Accept", "application/json");
    request.setTransferTimeout(7000);
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

    if (m_currentSource == Source::QuotesApi) {
        const QJsonArray quotes = obj["quotes"].toArray();

        if (quotes.isEmpty()) {
            emit errorOccurred("Сервер не вернул цитаты");
            return;
        }

        const int index = QRandomGenerator::global()->bounded(quotes.size());
        const QJsonObject quoteObject = quotes.at(index).toObject();

        m_pendingText = quoteObject["text"].toString().trimmed();
        m_pendingAuthor = quoteObject["author"].toObject()["name"].toString().trimmed();

        m_pendingTags.clear();
        const QJsonArray tags = quoteObject["tags"].toArray();
        for (const QJsonValue& tagValue : tags) {
            const QString tag = tagValue.toString().trimmed();
            if (!tag.isEmpty()) {
                m_pendingTags << tag;
            }
        }

        if (m_pendingText.isEmpty()) {
            emit errorOccurred("Сервер вернул пустую цитату");
            return;
        }

        if (m_pendingAuthor.isEmpty()) {
            m_pendingAuthor = "Unknown author";
        }

        translatePendingQuote();
        return;
    }

    if (m_currentSource == Source::TranslationApi) {
        const QJsonObject responseData = obj["responseData"].toObject();
        const QString text = responseData["translatedText"].toString().trimmed();

        if (text.isEmpty()) {
            emit errorOccurred("Не удалось перевести цитату на русский язык");
            return;
        }

        emitQuote(text, m_pendingAuthor, m_pendingTags);
    }
}

void QuoteClient::emitQuote(const QString& text, const QString& author, const QStringList& tags) {
    Quote quote;
    quote.text   = text;
    quote.author = author;
    quote.tags   = tags.join(", ");

    emit quoteReceived(quote);
}

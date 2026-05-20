#include "QuoteFetcher.h"

#include <QNetworkRequest>
#include <QUrl>
#include <QRegularExpression>
#include <QRandomGenerator>
#include <QTextDocument>

static const QString SOURCE_URL = "https://quotes.toscrape.com/";


QuoteFetcher::QuoteFetcher(QObject* parent)
    : QObject(parent)
    , m_manager(new QNetworkAccessManager(this))
{
    connect(m_manager, &QNetworkAccessManager::finished,
            this, &QuoteFetcher::onReplyFinished);
}

void QuoteFetcher::fetchQuotes() {
    QNetworkRequest request{QUrl(SOURCE_URL)};
    request.setHeader(QNetworkRequest::UserAgentHeader,
                      "Mozilla/5.0 (compatible; QtQuoteApp/1.0)");
    m_manager->get(request);
}

void QuoteFetcher::onReplyFinished(QNetworkReply* reply) {
    reply->deleteLater();

    if (reply->error() != QNetworkReply::NoError) {
        emit errorOccurred("Ошибка сети: " + reply->errorString());
        return;
    }

    const QString html = QString::fromUtf8(reply->readAll());
    m_quotes = parseQuotesFromHtml(html);

    if (m_quotes.isEmpty()) {
        emit errorOccurred("Не удалось найти цитаты на странице.");
        return;
    }

    emit quoteReady(pickRandomQuote());
}

QList<Quote> QuoteFetcher::parseQuotesFromHtml(const QString& html) {
    QList<Quote> result;

    QRegularExpression quoteRe(
        "<div[^>]*class=\"quote\"[^>]*>.*?"
        "<span[^>]*class=\"text\"[^>]*>(.*?)</span>.*?"
        "<small[^>]*class=\"author\"[^>]*>(.*?)</small>.*?"
        "<div[^>]*class=\"tags\"[^>]*>(.*?)</div>.*?</div>",
        QRegularExpression::DotMatchesEverythingOption |
        QRegularExpression::CaseInsensitiveOption
    );

    QRegularExpression tagRe("<a[^>]*class=\"tag\"[^>]*>(.*?)</a>",
                             QRegularExpression::DotMatchesEverythingOption |
                             QRegularExpression::CaseInsensitiveOption);

    auto it = quoteRe.globalMatch(html);
    while (it.hasNext()) {
        auto match = it.next();
        QString textHtml  = match.captured(1).trimmed();
        QString authorHtml = match.captured(2).trimmed();
        QString tagsHtml  = match.captured(3);

        QTextDocument textDoc;
        textDoc.setHtml(textHtml);
        const QString text = textDoc.toPlainText().trimmed();

        QTextDocument authorDoc;
        authorDoc.setHtml(authorHtml);
        QString author = authorDoc.toPlainText().trimmed();

        QStringList tags;
        auto tagIt = tagRe.globalMatch(tagsHtml);
        while (tagIt.hasNext()) {
            auto tagMatch = tagIt.next();
            const QString tagText = tagMatch.captured(1).trimmed();
            if (!tagText.isEmpty()) {
                tags << tagText;
            }
        }

        if (text.isEmpty()) continue;
        if (author.isEmpty()) author = "Неизвестный автор";

        Quote quote;
        quote.text = text;
        quote.author = author;
        quote.tags = tags.join(", ");
        result << quote;
    }

    return result;
}

Quote QuoteFetcher::pickRandomQuote() {
    const int idx = QRandomGenerator::global()->bounded(m_quotes.size());
    return m_quotes.at(idx);
}

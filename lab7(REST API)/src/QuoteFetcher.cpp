#include "QuoteFetcher.h"

#include <QNetworkRequest>
#include <QUrl>
#include <QRegularExpression>
#include <QRandomGenerator>

static const QString SOURCE_URL = "https://citaty.info/selection/citaty-stethema";


static const QStringList SECTION_HEADINGS = {
    "Жизненные цитаты Стэтхема",
    "Псевдофилософские цитаты Стэтхэма",
    "Настоящий цитатник Джейсона Стэтхэма"
};

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

QStringList QuoteFetcher::parseQuotesFromHtml(const QString& html) {
    QStringList result;

    QRegularExpression blockquoteRe(
        "<blockquote[^>]*>(.*?)</blockquote>",
        QRegularExpression::DotMatchesEverythingOption |
        QRegularExpression::CaseInsensitiveOption
    );

    QRegularExpression tagRe("<[^>]+>");

    auto it = blockquoteRe.globalMatch(html);
    while (it.hasNext()) {
        auto match = it.next();
        QString content = match.captured(1);
        content.remove(tagRe);
        content = content.trimmed();

        if (content.isEmpty()) continue;

        const QStringList lines = content.split('\n', Qt::SkipEmptyParts);
        for (const QString& line : lines) {
            const QString trimmed = line.trimmed();
            if (trimmed.isEmpty()) continue;
            if (SECTION_HEADINGS.contains(trimmed)) continue;
            result << trimmed;
        }
    }

    return result;
}

Quote QuoteFetcher::pickRandomQuote() {
    const int idx = QRandomGenerator::global()->bounded(m_quotes.size());

    Quote q;
    q.text   = m_quotes.at(idx);
    q.author = "Анатолий Пушкарский";
    q.tags   = "юмор, жизнь, философия";
    return q;
}

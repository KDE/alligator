/**
 * SPDX-FileCopyrightText: 2020 Tobias Fella <tobias.fella@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#include "entry.h"

#include <QDesktopServices>
#include <QRegularExpression>
#include <QSqlQuery>
#include <QUrl>

#include <KLocalizedString>

#include "database.h"

Entry::Entry(Feed *feed, int index)
    : QObject(nullptr)
    , m_feed(feed)
{
    QSqlQuery entryQuery;
    bool entryQueryPrepared = false;
    if (m_feed) {
        entryQueryPrepared = entryQuery.prepare(QStringLiteral("SELECT * FROM Entries WHERE feed=:feed ORDER BY updated DESC LIMIT 1 OFFSET :index;"));
        entryQuery.bindValue(QStringLiteral(":feed"), m_feed->url());
    } else {
        entryQueryPrepared = entryQuery.prepare(QStringLiteral("SELECT * FROM Entries ORDER BY updated DESC LIMIT 1 OFFSET :index;"));
    }
    if (entryQueryPrepared) {
        entryQuery.bindValue(QStringLiteral(":index"), index);
        Database::instance().execute(entryQuery);
        if (!entryQuery.next()) {
            qWarning() << "No element with index" << index << "found";
            if (m_feed) {
                qDebug() << " in feed" << m_feed->url();
            }
        }
    }

    QSqlQuery authorQuery;
    QStringList authors;
    if (authorQuery.prepare(QStringLiteral("SELECT * FROM Authors WHERE id=:id"))) {
        authorQuery.bindValue(QStringLiteral(":id"), entryQuery.value(QStringLiteral("id")).toString());
        Database::instance().execute(authorQuery);

        while (authorQuery.next()) {
            authors += authorQuery.value(QStringLiteral("name")).toString();
        }
    }

    if (authors.size() > 0) {
        m_authors = authors[0];
    }

    m_created.setSecsSinceEpoch(entryQuery.value(QStringLiteral("created")).toInt());
    m_updated.setSecsSinceEpoch(entryQuery.value(QStringLiteral("updated")).toInt());

    m_id = entryQuery.value(QStringLiteral("id")).toString();
    m_title = entryQuery.value(QStringLiteral("title")).toString();
    m_content = entryQuery.value(QStringLiteral("content")).toString();
    m_link = entryQuery.value(QStringLiteral("link")).toString();
    m_read = entryQuery.value(QStringLiteral("read")).toBool();
}

Entry::~Entry()
{
}

QString Entry::id() const
{
    return m_id;
}

QString Entry::title() const
{
    return m_title;
}

QString Entry::content() const
{
    return m_content;
}

QString Entry::authors() const
{
    return m_authors;
}

QDateTime Entry::created() const
{
    return m_created;
}

QDateTime Entry::updated() const
{
    return m_updated;
}

QString Entry::link() const
{
    return m_link;
}

bool Entry::read() const
{
    return m_read;
}

QString Entry::baseUrl() const
{
    return QUrl(m_link).adjusted(QUrl::RemovePath).toString();
}

void Entry::openLink(const QString &link)
{
    QUrl url(link);
    if (link.startsWith(QStringLiteral("//"))) {
        // we a protocol-relative, see https://en.wikipedia.org/wiki/Wikipedia:Protocol-relative_URL
        url.setScheme(QUrl(m_link).scheme());
    }
    QDesktopServices::openUrl(url);
}

void Entry::setRead(bool read)
{
    m_read = read;
    Q_EMIT readChanged(m_read);
    Database::instance().setRead(m_id, m_read);
    if (m_feed) {
        Q_EMIT m_feed->unreadEntryCountChanged();
    }
}

QString Entry::adjustedContent(int width, int fontSize)
{
    QString ret(m_content);
    QRegularExpression imgRegex(QStringLiteral("<img ((?!width=\"[0-9]+(px)?\").)*(width=\"([0-9]+)(px)?\")?[^>]*>"));

    QRegularExpressionMatchIterator i = imgRegex.globalMatch(ret);
    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();

        QString imgTag(match.captured());
        if (imgTag.contains(QStringLiteral("wp-smiley"))) {
            imgTag.insert(4, QStringLiteral(" width=\"%1\"").arg(fontSize));
        }

        QString widthParameter = match.captured(4);

        if (widthParameter.length() != 0) {
            if (widthParameter.toInt() > width) {
                imgTag.replace(match.captured(3), QStringLiteral("width=\"%1\"").arg(width));
                imgTag.replace(QRegularExpression(QStringLiteral("height=\"([0-9]+)(px)?\"")), QString());
            }
        } else {
            imgTag.insert(4, QStringLiteral(" width=\"%1\"").arg(width));
        }
        ret.replace(match.captured(), imgTag);
    }

    ret.replace(QStringLiteral("<img"), QStringLiteral("<br /> <img"));
    return ret;
}

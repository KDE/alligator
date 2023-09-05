/**
 * SPDX-FileCopyrightText: 2020 Tobias Fella <tobias.fella@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#include <KLocalizedString>
#include <QCryptographicHash>
#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QNetworkReply>
#include <QStandardPaths>
#include <QTextDocumentFragment>
#include <Syndication/Syndication>

#include "database.h"
#include "fetcher.h"

Fetcher::Fetcher()
    : m_fetchCount(0)
{
    manager = new QNetworkAccessManager(this);
    manager->setRedirectPolicy(QNetworkRequest::NoLessSafeRedirectPolicy);
    manager->setStrictTransportSecurityEnabled(true);
    manager->enableStrictTransportSecurityStore(true);
}

void Fetcher::fetch(const QString &url, const bool markEntriesRead)
{
    qDebug() << "Starting to fetch" << url;

    Q_EMIT startedFetchingFeed(url);
    setFetchCount(m_fetchCount + 1);

    QNetworkRequest request((QUrl(url)));
    QNetworkReply *reply = get(request);
    connect(reply, &QNetworkReply::finished, this, [this, url, reply, markEntriesRead]() {
        setFetchCount(m_fetchCount - 1);
        if (reply->error()) {
            qWarning() << "Error fetching feed";
            qWarning() << reply->errorString();
            Q_EMIT error(url, reply->error(), reply->errorString());
        } else {
            QByteArray data = reply->readAll();
            Syndication::DocumentSource document(data, url);
            Syndication::FeedPtr feed = Syndication::parserCollection()->parse(document, QStringLiteral("Atom"));
            processFeed(feed, url, markEntriesRead);
        }
        delete reply;
    });
}

void Fetcher::fetchAll()
{
    QSqlQuery query;
    query.prepare(QStringLiteral("SELECT url FROM Feeds;"));
    Database::instance().execute(query);
    while (query.next()) {
        fetch(query.value(0).toString());
    }
}

void Fetcher::setFetchCount(int count)
{
    m_fetchCount = count;
    Q_EMIT refreshingChanged(refreshing());
}

void Fetcher::processFeed(Syndication::FeedPtr feed, const QString &url, const bool markEntriesRead)
{
    if (feed.isNull()) {
        Syndication::ErrorCode errorCode = Syndication::parserCollection()->lastError();
        QString errorString = syndicationErrorToString(errorCode);
        Q_EMIT error(url, errorCode, errorString);
        return;
    }

    QSqlQuery query;
    query.prepare(QStringLiteral("UPDATE Feeds SET name=:name, image=:image, link=:link, description=:description, lastUpdated=:lastUpdated WHERE url=:url;"));
    query.bindValue(QStringLiteral(":name"), feed->title());
    query.bindValue(QStringLiteral(":url"), url);
    query.bindValue(QStringLiteral(":link"), feed->link());
    query.bindValue(QStringLiteral(":description"), feed->description());

    QDateTime current = QDateTime::currentDateTime();
    query.bindValue(QStringLiteral(":lastUpdated"), current.toSecsSinceEpoch());

    for (auto &author : feed->authors()) {
        processAuthor(author, QLatin1String(""), url);
    }

    QString imagePath;
    if (feed->image()->url().startsWith(QStringLiteral("/"))) {
        imagePath = QUrl(url).adjusted(QUrl::RemovePath).toString() + feed->image()->url();
    } else {
        imagePath = feed->image()->url();
    }
    query.bindValue(QStringLiteral(":image"), imagePath);
    Database::instance().execute(query);

    qDebug() << "Updated feed title:" << feed->title();

    Q_EMIT feedDetailsUpdated(url, feed->title(), imagePath, feed->link(), feed->description(), current);

    for (const auto &entry : feed->items()) {
        processEntry(entry, url, markEntriesRead);
    }

    Q_EMIT feedUpdated(url);
}

void Fetcher::processEntry(Syndication::ItemPtr entry, const QString &url, const bool markEntriesRead)
{
    qDebug() << "Processing" << entry->title();
    QSqlQuery query;
    query.prepare(QStringLiteral("SELECT COUNT (id) FROM Entries WHERE id=:id;"));
    query.bindValue(QStringLiteral(":id"), entry->id());
    Database::instance().execute(query);
    query.next();

    if (query.value(0).toInt() != 0) {
        return;
    }

    QDateTime current = QDateTime::currentDateTime();
    query.prepare(QStringLiteral("INSERT INTO Entries VALUES (:feed, :id, :title, :content, :created, :updated, :link, :read);"));
    query.bindValue(QStringLiteral(":feed"), url);
    query.bindValue(QStringLiteral(":id"), entry->id());
    query.bindValue(QStringLiteral(":title"), QTextDocumentFragment::fromHtml(entry->title()).toPlainText());

    if (entry->datePublished()) {
        query.bindValue(QStringLiteral(":created"), static_cast<int>(entry->datePublished()));
    } else {
        query.bindValue(QStringLiteral(":created"), current.toSecsSinceEpoch());
    }

    if (entry->dateUpdated()) {
        query.bindValue(QStringLiteral(":updated"), static_cast<int>(entry->dateUpdated()));
    } else {
        query.bindValue(QStringLiteral(":updated"), current.toSecsSinceEpoch());
    }

    query.bindValue(QStringLiteral(":link"), entry->link());
    query.bindValue(QStringLiteral(":read"), markEntriesRead);

    if (!entry->content().isEmpty()) {
        query.bindValue(QStringLiteral(":content"), entry->content());
    } else {
        query.bindValue(QStringLiteral(":content"), entry->description());
    }

    Database::instance().execute(query);

    for (const auto &author : entry->authors()) {
        processAuthor(author, entry->id(), url);
    }

    for (const auto &enclosure : entry->enclosures()) {
        processEnclosure(enclosure, entry, url);
    }
}

void Fetcher::processAuthor(Syndication::PersonPtr author, const QString &entryId, const QString &url)
{
    QSqlQuery query;
    query.prepare(QStringLiteral("INSERT INTO Authors VALUES(:feed, :id, :name, :uri, :email);"));
    query.bindValue(QStringLiteral(":feed"), url);
    query.bindValue(QStringLiteral(":id"), entryId);
    query.bindValue(QStringLiteral(":name"), author->name());
    query.bindValue(QStringLiteral(":uri"), author->uri());
    query.bindValue(QStringLiteral(":email"), author->email());
    Database::instance().execute(query);
}

void Fetcher::processEnclosure(Syndication::EnclosurePtr enclosure, Syndication::ItemPtr entry, const QString &feedUrl)
{
    QSqlQuery query;
    query.prepare(QStringLiteral("INSERT INTO Enclosures VALUES (:feed, :id, :duration, :size, :title, :type, :url);"));
    query.bindValue(QStringLiteral(":feed"), feedUrl);
    query.bindValue(QStringLiteral(":id"), entry->id());
    query.bindValue(QStringLiteral(":duration"), enclosure->duration());
    query.bindValue(QStringLiteral(":size"), enclosure->length());
    query.bindValue(QStringLiteral(":title"), enclosure->title());
    query.bindValue(QStringLiteral(":type"), enclosure->type());
    query.bindValue(QStringLiteral(":url"), enclosure->url());
    Database::instance().execute(query);
}

QString Fetcher::image(const QString &url)
{
    QString path = filePath(url);
    if (QFileInfo::exists(path)) {
        return path;
    }

    download(url);

    return QLatin1String("");
}

void Fetcher::download(const QString &url)
{
    QNetworkRequest request((QUrl(url)));
    QNetworkReply *reply = get(request);
    connect(reply, &QNetworkReply::finished, this, [this, url, reply]() {
        QByteArray data = reply->readAll();
        QFile file(filePath(url));
        file.open(QIODevice::WriteOnly);
        file.write(data);
        file.close();

        Q_EMIT imageDownloadFinished(url);
        delete reply;
    });
}

void Fetcher::removeImage(const QString &url)
{
    qDebug() << filePath(url);
    QFile(filePath(url)).remove();
}

QString Fetcher::filePath(const QString &url)
{
    return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + QStringLiteral("/")
        + QString::fromStdString(QCryptographicHash::hash(url.toUtf8(), QCryptographicHash::Md5).toHex().toStdString());
}

QNetworkReply *Fetcher::get(QNetworkRequest &request)
{
    request.setRawHeader("User-Agent", "Alligator/0.1; Syndication");
    return manager->get(request);
}

QString Fetcher::syndicationErrorToString(Syndication::ErrorCode errorCode)
{
    switch (errorCode) {
    case Syndication::InvalidXml:
        return i18n("Invalid XML");
    case Syndication::XmlNotAccepted:
        return i18n("No parser accepted the XML");
    default:
        return i18n("Error while parsing feed");
    }
}

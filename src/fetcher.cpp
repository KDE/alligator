/**
 * SPDX-FileCopyrightText: 2020 Tobias Fella <fella@posteo.de>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#include <QCryptographicHash>
#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QRegularExpression>
#include <QStandardPaths>
#include <QTextCodec>
#include <QTextDocumentFragment>

#include <Syndication/Syndication>

#include "database.h"
#include "fetcher.h"

Fetcher::Fetcher()
{
    manager = new QNetworkAccessManager(this);
    manager->setRedirectPolicy(QNetworkRequest::NoLessSafeRedirectPolicy);
    manager->setStrictTransportSecurityEnabled(true);
    manager->enableStrictTransportSecurityStore(true);
}

void Fetcher::fetch(const QString &url)
{
    qDebug() << "Starting to fetch" << url;

    Q_EMIT startedFetchingFeed(url);

    QNetworkRequest request((QUrl(url)));
    QNetworkReply *reply = get(request);
    connect(reply, &QNetworkReply::finished, this, [this, url, reply]() {
        if (reply->error()) {
            qWarning() << "Error fetching feed";
            qWarning() << reply->errorString();
            Q_EMIT error(url, reply->error(), reply->errorString());
        } else {
            QByteArray data = reply->readAll();
            Syndication::DocumentSource *document = new Syndication::DocumentSource(data, url);
            Syndication::FeedPtr feed = Syndication::parserCollection()->parse(*document, QStringLiteral("Atom"));
            processFeed(feed, url);
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

void Fetcher::processFeed(Syndication::FeedPtr feed, const QString &url)
{
    if (feed.isNull()) {
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

    QString image;
    if (feed->image()->url().startsWith(QStringLiteral("/"))) {
        image = QUrl(url).adjusted(QUrl::RemovePath).toString() + feed->image()->url();
    } else {
        image = feed->image()->url();
    }
    query.bindValue(QStringLiteral(":image"), image);
    Database::instance().execute(query);

    qDebug() << "Updated feed title:" << feed->title();

    Q_EMIT feedDetailsUpdated(url, feed->title(), image, feed->link(), feed->description(), current);

    for (const auto &entry : feed->items()) {
        processEntry(entry, url);
    }

    Q_EMIT feedUpdated(url);
}

void Fetcher::processEntry(Syndication::ItemPtr entry, const QString &url)
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

    query.prepare(QStringLiteral("INSERT INTO Entries VALUES (:feed, :id, :title, :content, :created, :updated, :link, 0);"));
    query.bindValue(QStringLiteral(":feed"), url);
    query.bindValue(QStringLiteral(":id"), entry->id());
    query.bindValue(QStringLiteral(":title"), QTextDocumentFragment::fromHtml(entry->title()).toPlainText());
    query.bindValue(QStringLiteral(":created"), static_cast<int>(entry->datePublished()));
    query.bindValue(QStringLiteral(":updated"), static_cast<int>(entry->dateUpdated()));
    query.bindValue(QStringLiteral(":link"), entry->link());

    if (!entry->content().isEmpty()) {
        query.bindValue(QStringLiteral(":content"), entry->content());
    } else {
        query.bindValue(QStringLiteral(":content"), entry->description());
    }

    // many feeds do not contain complete article, fetch content from link
    // TODO: make configurable
    QNetworkRequest request((QUrl(entry->link())));
    QNetworkReply *reply = get(request);
    connect(reply, &QNetworkReply::finished, this, [this, url, reply]() {
        if (reply->error()) {
            qWarning() << "Error fetching feed content";
            qWarning() << reply->errorString();
            Q_EMIT error(url, reply->error(), reply->errorString());
        } else {
            QString fetchedContent = QTextCodec::codecForMib(106)->toUnicode(reply->readAll());
            qDebug() << "Fetched content " << fetchedContent;

            // TODO: get text only
            // original: https://github.com/mozilla/readability
            // comparison to other solutions: https://github.com/awendland/readable-web-extractor-comparison
            // see https://github.com/dankito/Readability4J as used in https://github.com/FredJul/Flym/blob/171af1778a8cb14004dcc00ed04093862d8c154e/app/src/main/java/net/frju/flym/service/FetcherService.kt#L291
            //
            // While there are implementations in JS, Kotlin and Python, I couldn't find a library in C/C++.
            // possibilities:
            // - call JS from C++ (https://forum.qt.io/topic/79614/is-possible-to-call-from-c-a-javascript-function-inside-js-file/3, https://doc.qt.io/qt-5/qwebenginepage.html#runJavaScript)
            // - call https://github.com/eafer/rdrview in a subprocess
            // - extract a library from https://github.com/eafer/rdrview

            // TODO: improve html
            // see https://github.com/FredJul/Flym/blob/master/app/src/main/java/net/frju/flym/utils/HtmlUtils.kt
            
            // TODO: query.bindValue(QStringLiteral(":content"), final QString);
        }
        delete reply;
    });

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
    return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + QStringLiteral("/") + QString::fromStdString(QCryptographicHash::hash(url.toUtf8(), QCryptographicHash::Md5).toHex().toStdString());
}

QNetworkReply *Fetcher::get(QNetworkRequest &request)
{
    request.setRawHeader("User-Agent", "Alligator/0.1; Syndication");
    return manager->get(request);
}

/**
 * SPDX-FileCopyrightText: 2020 Tobias Fella <tobias.fella@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#include <KLocalizedString>
#include <QDateTime>
#include <QDir>
#include <QSqlDatabase>
#include <QSqlError>
#include <QStandardPaths>
#include <QUrl>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#include "alligatorsettings.h"
#include "database.h"
#include "debug.h"
#include "fetcher.h"

#define TRUE_OR_RETURN(x)                                                                                                                                      \
    if (!x)                                                                                                                                                    \
        return false;

Database::Database()
{
    QSqlDatabase db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"));
    QString databasePath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir(databasePath).mkpath(databasePath);
    db.setDatabaseName(databasePath + QStringLiteral("/database.db3"));
    if (!db.open()) {
        qCCritical(ALLIGATOR) << "Failed to open the database";
    }

    if (!migrateTo(3)) {
        qCritical(ALLIGATOR) << "Failed to migrate the database";
    }

    cleanup();
}

bool Database::migrateTo(const int targetVersion)
{
    if (version() >= targetVersion) {
        qCDebug(ALLIGATOR) << "Database already in version" << targetVersion;
        return true;
    }

    switch (targetVersion) {
    case 1:
        return migrateTo1();
    case 2:
        return migrateTo2();
    case 3:
        return migrateTo3();
    default:
        return true;
    }
}

bool Database::migrateTo3()
{
    migrateTo2();

    qDebug() << "Migrating database to version 3";
    TRUE_OR_RETURN(execute(QStringLiteral("ALTER TABLE Entries ADD COLUMN favorite BOOL DEFAULT FALSE;")));
    TRUE_OR_RETURN(execute(QStringLiteral("PRAGMA user_version = 3;")));
    return true;
}

bool Database::migrateTo2()
{
    migrateTo(1);

    qCDebug(ALLIGATOR) << "Migrating database to version 2";
    TRUE_OR_RETURN(execute(QStringLiteral("CREATE TABLE IF NOT EXISTS FeedGroups (name TEXT NOT NULL, description TEXT, defaultGroup INTEGER);")));
    TRUE_OR_RETURN(execute(QStringLiteral("ALTER TABLE Feeds ADD COLUMN groupName TEXT;")));
    TRUE_OR_RETURN(execute(QStringLiteral("ALTER TABLE Feeds ADD COLUMN displayName TEXT;")));
    auto dg = i18n("Default");
    TRUE_OR_RETURN(execute(QStringLiteral("INSERT INTO FeedGroups VALUES ('%1', '%2', 1);").arg(dg, i18n("Default Feed Group"))));
    TRUE_OR_RETURN(execute(QStringLiteral("UPDATE Feeds SET groupName = '%1';").arg(dg)));
    TRUE_OR_RETURN(execute(QStringLiteral("PRAGMA user_version = 2;")));

    return true;
}

bool Database::migrateTo1()
{
    qCDebug(ALLIGATOR) << "Migrating database to version 1";
    TRUE_OR_RETURN(
        execute(QStringLiteral("CREATE TABLE IF NOT EXISTS Feeds (name TEXT, url TEXT, image TEXT, link TEXT, description TEXT, deleteAfterCount INTEGER, "
                               "deleteAfterType INTEGER, subscribed INTEGER, lastUpdated INTEGER, notify BOOL);")));
    TRUE_OR_RETURN(execute(QStringLiteral(
        "CREATE TABLE IF NOT EXISTS Entries (feed TEXT, id TEXT UNIQUE, title TEXT, content TEXT, created INTEGER, updated INTEGER, link TEXT, read bool);")));
    TRUE_OR_RETURN(execute(QStringLiteral("CREATE TABLE IF NOT EXISTS Authors (feed TEXT, id TEXT, name TEXT, uri TEXT, email TEXT);")));
    TRUE_OR_RETURN(execute(
        QStringLiteral("CREATE TABLE IF NOT EXISTS Enclosures (feed TEXT, id TEXT, duration INTEGER, size INTEGER, title TEXT, type STRING, url STRING);")));
    TRUE_OR_RETURN(execute(QStringLiteral("PRAGMA user_version = 1;")));
    return true;
}

bool Database::execute(const QString &query)
{
    QSqlQuery q;
    q.prepare(query);
    return execute(q);
}

bool Database::execute(QSqlQuery &query)
{
    if (!query.exec()) {
        qCWarning(ALLIGATOR) << "Failed to execute SQL Query";
        qCWarning(ALLIGATOR) << query.lastQuery();
        qCWarning(ALLIGATOR) << query.lastError();
        return false;
    }
    return true;
}

int Database::version()
{
    QSqlQuery query;
    query.prepare(QStringLiteral("PRAGMA user_version;"));
    execute(query);
    if (!query.next()) {
        return -1;
    }
    int value = query.value(0).toInt();
    qCDebug(ALLIGATOR) << "Database version " << value;
    return value;
}

void Database::cleanup()
{
    int count = AlligatorSettings::self()->deleteAfterCount();
    int type = AlligatorSettings::self()->deleteAfterType();

    if (type == 0) { // Never delete Entries
        return;
    }

    if (type == 1) { // Delete after <count> posts per feed
        // TODO
    } else {
        QDateTime dateTime = QDateTime::currentDateTime();
        if (type == 2) {
            dateTime = dateTime.addDays(-count);
        } else if (type == 3) {
            dateTime = dateTime.addDays(-7 * count);
        } else if (type == 4) {
            dateTime = dateTime.addMonths(-count);
        }
        qint64 sinceEpoch = dateTime.toSecsSinceEpoch();

        QSqlQuery query;
        query.prepare(QStringLiteral("DELETE FROM Entries WHERE updated < :sinceEpoch AND favorite != 1;"));
        query.bindValue(QStringLiteral(":sinceEpoch"), sinceEpoch);
        execute(query);
    }
}

bool Database::feedExists(const QString &url)
{
    QSqlQuery query;
    query.prepare(QStringLiteral("SELECT COUNT (url) FROM Feeds WHERE url=:url;"));
    query.bindValue(QStringLiteral(":url"), url);
    Database::instance().execute(query);
    query.next();
    return query.value(0).toInt() != 0;
}

void Database::addFeed(const QString &url, const QString &groupName, const bool markEntriesRead)
{
    qCDebug(ALLIGATOR) << "Adding feed";
    if (feedExists(url)) {
        qCDebug(ALLIGATOR) << "Feed already exists";
        return;
    }
    qCDebug(ALLIGATOR) << "Feed does not yet exist";

    QUrl urlFromInput = QUrl::fromUserInput(url);
    QSqlQuery query;
    query.prepare(
        QStringLiteral("INSERT INTO Feeds VALUES (:name, :url, :image, :link, :description, :deleteAfterCount, :deleteAfterType, :subscribed, "
                       ":lastUpdated, :notify, :groupName, :displayName);"));
    query.bindValue(QStringLiteral(":name"), urlFromInput.toString());
    query.bindValue(QStringLiteral(":url"), urlFromInput.toString());
    query.bindValue(QStringLiteral(":image"), QLatin1String(""));
    query.bindValue(QStringLiteral(":link"), QLatin1String(""));
    query.bindValue(QStringLiteral(":description"), QLatin1String(""));
    query.bindValue(QStringLiteral(":deleteAfterCount"), 0);
    query.bindValue(QStringLiteral(":deleteAfterType"), 0);
    query.bindValue(QStringLiteral(":subscribed"), QDateTime::currentDateTime().toSecsSinceEpoch());
    query.bindValue(QStringLiteral(":lastUpdated"), 0);
    query.bindValue(QStringLiteral(":notify"), false);
    query.bindValue(QStringLiteral(":groupName"), groupName.isEmpty() ? defaultGroup() : groupName);
    query.bindValue(QStringLiteral(":displayName"), QLatin1String(""));
    execute(query);
    Q_EMIT feedAdded(urlFromInput.toString());
    Fetcher::instance().fetch(urlFromInput.toString(), markEntriesRead);
}

void Database::importFeeds(const QString &path)
{
    QUrl url(path);
    QFile file(url.isLocalFile() ? url.toLocalFile() : url.toString());
    file.open(QIODevice::ReadOnly);

    QXmlStreamReader xmlReader(&file);
    while (!xmlReader.atEnd()) {
        xmlReader.readNext();
        if (xmlReader.tokenType() == 4 && xmlReader.attributes().hasAttribute(QStringLiteral("xmlUrl"))) {
            addFeed(xmlReader.attributes().value(QStringLiteral("xmlUrl")).toString());
        }
    }
    Fetcher::instance().fetchAll();
}

void Database::exportFeeds(const QString &path)
{
    QUrl url(path);
    QFile file(url.isLocalFile() ? url.toLocalFile() : url.toString());
    file.open(QIODevice::WriteOnly);

    QXmlStreamWriter xmlWriter(&file);
    xmlWriter.setAutoFormatting(true);
    xmlWriter.writeStartDocument(QStringLiteral("1.0"));
    xmlWriter.writeStartElement(QStringLiteral("opml"));
    xmlWriter.writeEmptyElement(QStringLiteral("head"));
    xmlWriter.writeStartElement(QStringLiteral("body"));
    xmlWriter.writeAttribute(QStringLiteral("version"), QStringLiteral("1.0"));
    QSqlQuery query;
    query.prepare(QStringLiteral("SELECT url, name FROM Feeds;"));
    execute(query);
    while (query.next()) {
        xmlWriter.writeEmptyElement(QStringLiteral("outline"));
        xmlWriter.writeAttribute(QStringLiteral("xmlUrl"), query.value(0).toString());
        xmlWriter.writeAttribute(QStringLiteral("title"), query.value(1).toString());
    }
    xmlWriter.writeEndElement();
    xmlWriter.writeEndElement();
    xmlWriter.writeEndDocument();
}

void Database::addFeedGroup(const QString &name, const QString &description, const int isDefault)
{
    if (feedGroupExists(name)) {
        qCDebug(ALLIGATOR) << "Feed group already exists, nothing to add";
        return;
    }

    QSqlQuery query;
    query.prepare(QStringLiteral("INSERT INTO FeedGroups VALUES (:name, :desc, :isDefault);"));
    query.bindValue(QStringLiteral(":name"), name);
    query.bindValue(QStringLiteral(":desc"), description);
    query.bindValue(QStringLiteral(":isDefault"), isDefault);
    execute(query);

    Q_EMIT feedGroupsUpdated();
}

void Database::editFeed(const QString &url, const QString &displayName, const QString &groupName)
{
    QSqlQuery query;
    query.prepare(QStringLiteral("UPDATE Feeds SET displayName = :displayName, groupName = :groupName WHERE url = :url;"));
    query.bindValue(QStringLiteral(":displayName"), displayName);
    query.bindValue(QStringLiteral(":groupName"), groupName);
    query.bindValue(QStringLiteral(":url"), url);
    execute(query);

    Q_EMIT feedDetailsUpdated(url, displayName, groupName);
}

void Database::removeFeedGroup(const QString &name)
{
    clearFeedGroup(name);

    QSqlQuery query;
    query.prepare(QStringLiteral("DELETE FROM FeedGroups WHERE name = :name;"));
    query.bindValue(QStringLiteral(":name"), name);
    execute(query);

    Q_EMIT feedGroupRemoved(name);
}

void Database::setRead(const QString &entryId, bool read)
{
    QSqlQuery query;
    query.prepare(QStringLiteral("UPDATE Entries SET read=:read WHERE id=:id"));
    query.bindValue(QStringLiteral(":id"), entryId);
    query.bindValue(QStringLiteral(":read"), read);
    execute(query);

    Q_EMIT entryReadChanged(entryId, read);
}

void Database::setFeedRead(const QString &url, bool read)
{
    QSqlQuery query;
    query.prepare(QStringLiteral("UPDATE Entries SET read=:read WHERE feed=:url"));
    query.bindValue(QStringLiteral(":url"), url);
    query.bindValue(QStringLiteral(":read"), read);
    execute(query);

    Q_EMIT feedReadChanged();
}

void Database::setAllRead(bool read)
{
    QSqlQuery query;
    query.prepare(QStringLiteral("UPDATE Entries SET read=:read"));
    query.bindValue(QStringLiteral(":read"), read);
    execute(query);

    Q_EMIT feedReadChanged();
}

void Database::setFavorite(const QString &entryId, bool favorite)
{
    QSqlQuery query;
    query.prepare(QStringLiteral("UPDATE Entries SET favorite=:favorite WHERE id=:id"));
    query.bindValue(QStringLiteral(":id"), entryId);
    query.bindValue(QStringLiteral(":favorite"), favorite);
    execute(query);

    Q_EMIT entryFavoriteChanged(entryId, favorite);
}

bool Database::feedGroupExists(const QString &name)
{
    QSqlQuery query;
    query.prepare(QStringLiteral("SELECT COUNT (1) FROM FeedGroups WHERE name = :name;"));
    query.bindValue(QStringLiteral(":name"), name);
    Database::instance().execute(query);
    query.next();
    return query.value(0).toInt() != 0;
}

void Database::clearFeedGroup(const QString &name)
{
    QSqlQuery query;
    query.prepare(QStringLiteral("UPDATE Feeds SET groupName = NULL WHERE groupName = :name;"));
    query.bindValue(QStringLiteral(":name"), name);
    execute(query);
}

QString Database::defaultGroup()
{
    QSqlQuery query;
    query.prepare(QStringLiteral("SELECT Name FROM FeedGroups WHERE defaultGroup = 1"));
    execute(query);

    if (query.next()) {
        return query.value(0).toString();
    }
    auto dg = i18n("Default");
    addFeedGroup(dg, i18n("Default Feed Group"), 1);
    return dg;
}

void Database::setDefaultGroup(const QString &name)
{
    execute(QStringLiteral("UPDATE FeedGroups SET defaultGroup = 0;"));
    QSqlQuery query;
    query.prepare(QStringLiteral("UPDATE FeedGroups SET defaultGroup = 1 WHERE name=:name;"));
    query.bindValue(QStringLiteral(":name"), name);
    execute(query);
    Q_EMIT feedGroupsUpdated();
}

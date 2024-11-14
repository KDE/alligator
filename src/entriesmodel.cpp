/**
 * SPDX-FileCopyrightText: 2020 Tobias Fella <tobias.fella@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#include <QVariant>
#include <QVector>

#include "database.h"
#include "debug.h"
#include "entriesmodel.h"
#include "fetcher.h"

EntriesModel::EntriesModel(QObject *parent)
    : QAbstractListModel(parent)
{
    connect(&Fetcher::instance(), &Fetcher::feedUpdated, this, [this](const QString &url) {
        if (m_feedUrl.isEmpty() || m_feedUrl == url) {
            beginResetModel();
            m_entries.clear();
            loadEntries();
            endResetModel();
        }
    });
    connect(&Database::instance(), &Database::entryReadChanged, this, [this](const QString &entryId, bool read) {
        for (auto i = 0; i < m_entries.size(); i++) {
            if (m_entries[i].id == entryId) {
                m_entries[i].read = read;
                dataChanged(index(i, 0), index(i, 0), {ReadRole});
                break;
            }
        }
    });
    connect(&Database::instance(), &Database::entryFavoriteChanged, this, [this](const QString &entryId, bool favorite) {
        for (auto i = 0; i < m_entries.size(); i++) {
            if (m_entries[i].id == entryId) {
                m_entries[i].favorite = favorite;
                dataChanged(index(i, 0), index(i, 0), {FavoritesRole});
                break;
            }
        }
    });
    loadEntries();
}

QVariant EntriesModel::data(const QModelIndex &index, int role) const
{
    const auto &entry = m_entries[index.row()];
    if (role == IdRole) {
        return entry.id;
    }
    if (role == ReadRole) {
        return entry.read;
    }
    if (role == TitleRole) {
        return entry.title;
    }
    if (role == ContentRole) {
        return entry.content;
    }
    if (role == AuthorsRole) {
        return entry.authors;
    }
    if (role == CreatedRole) {
        return entry.created;
    }
    if (role == UpdatedRole) {
        return entry.updated;
    }
    if (role == LinkRole) {
        return entry.link;
    }
    if (role == BaseUrlRole) {
        return QUrl(entry.link).adjusted(QUrl::RemovePath).toString();
    }
    if (role == FavoritesRole) {
        return entry.favorite;
    }
    return QVariant();
}

QHash<int, QByteArray> EntriesModel::roleNames() const
{
    return {
        {IdRole, "id"},
        {ReadRole, "read"},
        {TitleRole, "title"},
        {ContentRole, "content"},
        {AuthorsRole, "authors"},
        {CreatedRole, "created"},
        {UpdatedRole, "updated"},
        {LinkRole, "link"},
        {BaseUrlRole, "baseUrl"},
        {ReadRole, "read"},
        {FavoritesRole, "favorite"},
    };
}

int EntriesModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    QSqlQuery query;
    if (m_feedUrl.length() > 0) {
        query.prepare(QStringLiteral("SELECT COUNT() FROM Entries WHERE feed=:feed;"));
        query.bindValue(QStringLiteral(":feed"), m_feedUrl);
    } else {
        query.prepare(QStringLiteral("SELECT COUNT() FROM Entries;"));
    }
    Database::instance().execute(query);
    if (!query.next()) {
        qCWarning(ALLIGATOR) << "Failed to query feed count";
    }
    return query.value(0).toInt();
}

void EntriesModel::loadEntries()
{
    QSqlQuery entryQuery;
    beginResetModel();
    m_entries.clear();
    if (m_feedUrl.length() > 0) {
        entryQuery.prepare(QStringLiteral("SELECT * FROM Entries WHERE feed=:feed ORDER BY updated DESC"));
        entryQuery.bindValue(QStringLiteral(":feed"), m_feedUrl);
    } else {
        entryQuery.prepare(QStringLiteral("SELECT * FROM Entries ORDER BY updated DESC"));
    }
    Database::instance().execute(entryQuery);

    while (entryQuery.next()) {
        Entry entry;
        QSqlQuery authorQuery;
        QStringList authorList;
        authorQuery.prepare(QStringLiteral("SELECT * FROM Authors WHERE id=:id"));
        authorQuery.bindValue(QStringLiteral(":id"), entryQuery.value(QStringLiteral("id")).toString());
        Database::instance().execute(authorQuery);

        while (authorQuery.next()) {
            authorList += authorQuery.value(QStringLiteral("name")).toString();
        }

        if (authorList.size() > 0) {
            entry.authors = authorList[0];
        }

        entry.created.setSecsSinceEpoch(entryQuery.value(QStringLiteral("created")).toInt());
        entry.updated.setSecsSinceEpoch(entryQuery.value(QStringLiteral("updated")).toInt());
        entry.id = entryQuery.value(QStringLiteral("id")).toString();
        entry.title = entryQuery.value(QStringLiteral("title")).toString();
        entry.content = entryQuery.value(QStringLiteral("content")).toString();
        entry.link = entryQuery.value(QStringLiteral("link")).toString();
        entry.read = entryQuery.value(QStringLiteral("read")).toBool();
        entry.favorite = entryQuery.value((QStringLiteral("favorite"))).toBool();

        m_entries.append(entry);
    }
    endResetModel();
}

QString EntriesModel::feedUrl() const
{
    return m_feedUrl;
}

void EntriesModel::setFeedUrl(const QString &feedUrl)
{
    if (feedUrl == m_feedUrl) {
        return;
    }
    m_feedUrl = feedUrl;
    Q_EMIT feedUrlChanged();
    loadEntries();
}

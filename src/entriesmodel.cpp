/**
 * SPDX-FileCopyrightText: 2020 Tobias Fella <fella@posteo.de>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#include <QAbstractListModel>
#include <QVariant>
#include <QVector>

#include "database.h"
#include "entriesmodel.h"
#include "fetcher.h"

EntriesModel::EntriesModel(Feed *feed)
    : QAbstractListModel(feed)
    , m_feed(feed)
{
    connect(&Fetcher::instance(), &Fetcher::feedUpdated, this, [this](const QString & url) {
        if (m_feed->url() == url) {
            beginResetModel();
            for (auto &entry : m_entries) {
                delete entry;
            }
            m_entries.clear();
            endResetModel();
        }
    });
}

EntriesModel::~EntriesModel()
{
    qDeleteAll(m_entries);
}

QVariant EntriesModel::data(const QModelIndex &index, int role) const
{
    if (role != 0) {
        return QVariant();
    }
    if (m_entries[index.row()] == nullptr) {
        loadEntry(index.row());
    }
    return QVariant::fromValue(m_entries[index.row()]);
}

QHash<int, QByteArray> EntriesModel::roleNames() const
{
    QHash<int, QByteArray> roleNames;
    roleNames[0] = "entry";
    return roleNames;
}

int EntriesModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    QSqlQuery query;
    if (m_feed) {
        query.prepare(QStringLiteral("SELECT COUNT() FROM Entries WHERE feed=:feed;"));
        query.bindValue(QStringLiteral(":feed"), m_feed->url());
    } else {
        query.prepare(QStringLiteral("SELECT COUNT() FROM Entries;"));
    }
    Database::instance().execute(query);
    if (!query.next()) {
        qWarning() << "Failed to query feed count";
    }
    return query.value(0).toInt();
}

void EntriesModel::loadEntry(int index) const
{
    m_entries[index] = new Entry(m_feed, index);
}

Feed *EntriesModel::feed() const
{
    return m_feed;
}

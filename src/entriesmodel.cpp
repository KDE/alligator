/**
 * SPDX-FileCopyrightText: 2020 Tobias Fella <tobias.fella@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#include <QVariant>
#include <QVector>

#include "database.h"
#include "entriesmodel.h"
#include "fetcher.h"

EntriesModel::EntriesModel(QObject *parent)
    : QAbstractListModel(parent)
{
    connect(&Fetcher::instance(), &Fetcher::feedUpdated, this, [this](const QString &url) {
        if (m_feedUrl.isEmpty() || m_feedUrl == url) {
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
    if (m_feedUrl.length() > 0) {
        query.prepare(QStringLiteral("SELECT COUNT() FROM Entries WHERE feed=:feed;"));
        query.bindValue(QStringLiteral(":feed"), m_feedUrl);
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
    m_entries[index] = new Entry(m_feedUrl, index);
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
}

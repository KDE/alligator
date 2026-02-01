/**
 * SPDX-FileCopyrightText: 2020 Tobias Fella <tobias.fella@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#include <QDebug>
#include <QModelIndex>
#include <QSqlQuery>
#include <QVariant>

#include "database.h"
#include "debug.h"
#include "feedsmodel.h"
#include "fetcher.h"

FeedsModel::FeedsModel(QObject *parent)
    : QAbstractListModel(parent)
{
    connect(&Database::instance(), &Database::feedAdded, this, [this]() {
        beginInsertRows(QModelIndex(), rowCount(QModelIndex()) - 1, rowCount(QModelIndex()) - 1);
        endInsertRows();
    });

    connect(
        &Fetcher::instance(),
        &Fetcher::feedDetailsUpdated,
        this,
        [this](const QString &url, const QString &name, const QString &image, const QString &link, const QString &description, const QDateTime &lastUpdated) {
            for (size_t i = 0; i < m_feeds.size(); i++) {
                if (m_feeds[i]->url() == url) {
                    m_feeds[i]->setName(name);
                    m_feeds[i]->setImage(image);
                    m_feeds[i]->setLink(link);
                    m_feeds[i]->setDescription(description);
                    m_feeds[i]->setLastUpdated(lastUpdated);
                    Q_EMIT dataChanged(createIndex(i, 0), createIndex(i, 0));
                    break;
                }
            }
        });

    connect(&Database::instance(), &Database::feedDetailsUpdated, [this](const QString &url, const QString &displayName, const QString &groupName) {
        for (size_t i = 0; i < m_feeds.size(); i++) {
            if (m_feeds[i]->url() == url) {
                m_feeds[i]->setDisplayName(displayName);
                m_feeds[i]->setGroupName(groupName);
                Q_EMIT dataChanged(createIndex(i, 0), createIndex(i, 0));
                break;
            }
        }
    });

    connect(&Database::instance(), &Database::feedGroupRemoved, [this](const QString &groupName) {
        for (size_t i = 0; i < m_feeds.size(); i++) {
            if (m_feeds[i]->groupName() == groupName) {
                m_feeds[i]->setGroupName(QString());
                Q_EMIT dataChanged(createIndex(i, 0), createIndex(i, 0));
                break;
            }
        }
    });
}

QHash<int, QByteArray> FeedsModel::roleNames() const
{
    QHash<int, QByteArray> roleNames;
    roleNames[0] = "feed";
    return roleNames;
}

int FeedsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    QSqlQuery query;
    query.prepare(QStringLiteral("SELECT COUNT() FROM Feeds;"));
    Database::instance().execute(query);
    if (!query.next()) {
        qCWarning(ALLIGATOR) << "Failed to query feed count";
    }
    return query.value(0).toInt();
}

QVariant FeedsModel::data(const QModelIndex &index, int role) const
{
    if (role != 0) {
        return QVariant();
    }

    while (m_feeds.size() <= index.row()) {
        loadFeed(m_feeds.size());
    }
    return QVariant::fromValue(m_feeds[index.row()].get());
}

void FeedsModel::loadFeed(int index) const
{
    m_feeds.push_back(std::make_unique<Feed>(index));
}

void FeedsModel::removeFeed(const QString &url)
{
    for (size_t i = 0; i < m_feeds.size(); i++) {
        if (m_feeds[i]->url() == url) {
            m_feeds[i]->remove();
            beginRemoveRows(QModelIndex(), i, i);
            m_feeds.erase(m_feeds.cbegin() + i);
            endRemoveRows();
        }
    }
}

void FeedsModel::refreshAll()
{
    for (auto &feed : m_feeds) {
        feed->refresh();
    }
}

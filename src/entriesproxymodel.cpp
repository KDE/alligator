/*
 * SPDX-FileCopyrightText: 2021 Dimitris Kardarakos <dimkard@posteo.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "entriesproxymodel.h"
#include "database.h"
#include "entriesmodel.h"

EntriesProxyModel::EntriesProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
    , m_onlyUnread(false)
    , m_onlyFavorite(false)
{
    connect(&Database::instance(), &Database::entryReadChanged, this, [this]() {
        invalidateFilter();
    });
    connect(&Database::instance(), &Database::feedReadChanged, this, [this]() {
        invalidateFilter();
    });
    connect(&Database::instance(), &Database::entryFavoriteChanged, this, [this]() {
        invalidateFilter();
    });
}

EntriesProxyModel::~EntriesProxyModel()
{
}

void EntriesProxyModel::setOnlyUnread(bool onlyUnread)
{
    if (m_onlyUnread != onlyUnread) {
        m_onlyUnread = onlyUnread;
        invalidateFilter();
        Q_EMIT onlyUnreadChanged();
    }
}

void EntriesProxyModel::setOnlyFavorite(bool onlyFavorite)
{
    if (m_onlyFavorite == onlyFavorite) {
        return;
    }
    m_onlyFavorite = onlyFavorite;
    invalidateFilter();
    Q_EMIT onlyFavoriteChanged();
}

bool EntriesProxyModel::onlyFavorite() const
{
    return m_onlyFavorite;
}

bool EntriesProxyModel::onlyUnread() const
{
    return m_onlyUnread;
}

bool EntriesProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    const auto idx = sourceModel()->index(source_row, 0, source_parent);

    if (!QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent)) {
        return false;
    }

    if (m_onlyUnread) {
        return !idx.data(EntriesModel::ReadRole).value<bool>();
    }

    if (m_onlyFavorite) {
        return idx.data(EntriesModel::FavoritesRole).value<bool>();
    }

    return true;
}

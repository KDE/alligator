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
        slotInvalidateFilter();
    });
    connect(&Database::instance(), &Database::feedReadChanged, this, [this]() {
        slotInvalidateFilter();
    });
    connect(&Database::instance(), &Database::entryFavoriteChanged, this, [this]() {
        slotInvalidateFilter();
    });
}

EntriesProxyModel::~EntriesProxyModel()
{
}

void EntriesProxyModel::slotInvalidateFilter()
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 10, 0)
    beginFilterChange();
    endFilterChange(QSortFilterProxyModel::Direction::Rows);
#else
    invalidateFilter();
#endif
}

void EntriesProxyModel::setOnlyUnread(bool onlyUnread)
{
    if (m_onlyUnread != onlyUnread) {
#if QT_VERSION >= QT_VERSION_CHECK(6, 10, 0)
        beginFilterChange();
#endif
        m_onlyUnread = onlyUnread;
#if QT_VERSION >= QT_VERSION_CHECK(6, 10, 0)
        endFilterChange(QSortFilterProxyModel::Direction::Rows);
#else
        invalidateFilter();
#endif
        Q_EMIT onlyUnreadChanged();
    }
}

void EntriesProxyModel::setOnlyFavorite(bool onlyFavorite)
{
    if (m_onlyFavorite == onlyFavorite) {
        return;
    }

#if QT_VERSION >= QT_VERSION_CHECK(6, 10, 0)
    beginFilterChange();
#endif
    m_onlyFavorite = onlyFavorite;
#if QT_VERSION >= QT_VERSION_CHECK(6, 10, 0)
    endFilterChange(QSortFilterProxyModel::Direction::Rows);
#else
    invalidateFilter();
#endif
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

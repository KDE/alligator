/*
* SPDX-FileCopyrightText: 2021 Dimitris Kardarakos <dimkard@posteo.net>
*
* SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "entriesproxymodel.h"
#include "entry.h"
#include "database.h"

EntriesProxyModel::EntriesProxyModel(QObject *parent) : QSortFilterProxyModel(parent), m_onlyUnread(false)
{
    connect(&Database::instance(), &Database::entryReadChanged, this, [this]() {
        invalidateFilter();
    });
}

EntriesProxyModel::~EntriesProxyModel()
{}

void EntriesProxyModel::setOnlyUnread(bool onlyUnread)
{
    if (m_onlyUnread != onlyUnread) {
        m_onlyUnread = onlyUnread;
        invalidateFilter();
        Q_EMIT onlyUnreadChanged();
    }
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

    if (!m_onlyUnread) {
        return true;
    }

    auto entry = idx.data(0).value<Entry *>();

    return !entry->read();

    return false;
}

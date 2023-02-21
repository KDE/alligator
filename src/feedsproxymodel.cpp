/*
 * SPDX-FileCopyrightText: 2021 Dimitris Kardarakos <dimkard@posteo.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "feedsproxymodel.h"
#include "database.h"
#include "feed.h"
#include "feedsmodel.h"

FeedsProxyModel::FeedsProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
    , m_group_name{}
{
    connect(&Database::instance(), &Database::feedDetailsUpdated, [this]() {
        invalidate();
    });
    setSortRole(0);
    sort(0);
}

FeedsProxyModel::~FeedsProxyModel()
{
}

void FeedsProxyModel::setGroupName(const QString &name)
{
    if (m_group_name != name) {
        m_group_name = name;
        invalidateFilter();
        Q_EMIT groupNameChanged();
    }
}

QString FeedsProxyModel::groupName() const
{
    return m_group_name;
}

bool FeedsProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    const auto idx = sourceModel()->index(source_row, 0, source_parent);

    if (!QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent)) {
        return false;
    }

    if (m_group_name.isEmpty()) {
        return true;
    }

    auto feed = idx.data(0).value<Feed *>();

    if (feed->groupName() == m_group_name) {
        return true;
    }

    return false;
}

bool FeedsProxyModel::lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const
{
    if (!sourceModel()) {
        return false;
    }
    const auto leftFeed = source_left.data(0).value<Feed *>();
    const auto rightFeed = source_right.data(0).value<Feed *>();

    return leftFeed->groupName() < rightFeed->groupName();
}

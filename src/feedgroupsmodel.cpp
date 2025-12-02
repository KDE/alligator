/*
 * SPDX-FileCopyrightText: 2021 Dimitris Kardarakos <dimkard@posteo.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "feedgroupsmodel.h"
#include "database.h"
#include <QSqlQuery>

FeedGroupsModel::FeedGroupsModel(QObject *parent)
    : QAbstractListModel{parent}
    , m_feed_groups{}
{
    loadFromDatabase();

    connect(&Database::instance(), &Database::feedGroupsUpdated, this, &FeedGroupsModel::loadFromDatabase);

    connect(&Database::instance(), &Database::feedGroupRemoved, this, &FeedGroupsModel::loadFromDatabase);
}

QHash<int, QByteArray> FeedGroupsModel::roleNames() const
{
    return {{GroupName, "name"}, {GroupDescription, "description"}, {IsDefault, "isDefault"}};
}

int FeedGroupsModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }

    return m_feed_groups.count();
}

QVariant FeedGroupsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    auto row = index.row();

    switch (role) {
    case GroupName: {
        return m_feed_groups.at(row).name;
    }
    case GroupDescription: {
        return m_feed_groups.at(row).description;
    }
    case IsDefault:
        return m_feed_groups.at(row).isDefault;
    }

    return QVariant();
}

void FeedGroupsModel::loadFromDatabase()
{
    beginResetModel();

    m_feed_groups = {};
    QSqlQuery q;
    q.prepare(QStringLiteral("SELECT * FROM FeedGroups;"));
    Database::instance().execute(q);
    while (q.next()) {
        FeedGroup group{};
        group.name = q.value(QStringLiteral("name")).toString();
        group.description = q.value(QStringLiteral("description")).toString();
        group.isDefault = (q.value(QStringLiteral("defaultGroup")).toInt() == 1);
        m_feed_groups << group;
    }

    endResetModel();
}

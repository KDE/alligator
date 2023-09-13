/*
 * SPDX-FileCopyrightText: 2021 Dimitris Kardarakos <dimkard@posteo.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <QAbstractListModel>
#include <QQmlEngine>

/**
 * @brief Data structure to store a single feed group entry
 *
 */
struct FeedGroup {
    QString name;
    QString description;
    bool isDefault;
};

/**
 * @brief Model that provides the feed groups
 *
 */
class FeedGroupsModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT

public:
    explicit FeedGroupsModel(QObject *parent = nullptr);

    enum RoleNames { GroupName = Qt::UserRole + 1, GroupDescription, IsDefault };

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex &parent) const override;

private:
    void loadFromDatabase();
    QVector<FeedGroup> m_feed_groups;
};

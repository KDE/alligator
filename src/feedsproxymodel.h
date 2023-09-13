/*
 * SPDX-FileCopyrightText: 2021 Dimitris Kardarakos <dimkard@posteo.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <QQmlEngine>
#include <QSortFilterProxyModel>

/**
 * @brief Filters and sorts FeedsModel
 *
 */
class FeedsProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QString groupName READ groupName WRITE setGroupName NOTIFY groupNameChanged)

public:
    explicit FeedsProxyModel(QObject *parent = nullptr);
    ~FeedsProxyModel() override;

    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;
    bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const override;

    QString groupName() const;
    void setGroupName(const QString &name);

Q_SIGNALS:
    void groupNameChanged();

private:
    QString m_group_name;
};

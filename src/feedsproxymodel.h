/*
* SPDX-FileCopyrightText: 2021 Dimitris Kardarakos <dimkard@posteo.net>
*
* SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef FEEDS_PROXY_MODEL_H
#define FEEDS_PROXY_MODEL_H

#include <QSortFilterProxyModel>

/**
 * @brief Filters and sorts FeedsModel
 *
 */
class FeedsProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

    Q_PROPERTY(QString groupName READ groupName WRITE setGroupName NOTIFY groupNameChanged)

public:
    explicit FeedsProxyModel(QObject *parent = nullptr);
    ~FeedsProxyModel() override;

    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;

    QString groupName() const;
    void setGroupName(const QString &name);

Q_SIGNALS:
    void groupNameChanged();

private:
    QString m_group_name;
};
#endif

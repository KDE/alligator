/*
 * SPDX-FileCopyrightText: 2021 Dimitris Kardarakos <dimkard@posteo.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <QSortFilterProxyModel>

/**
 * @brief Filters EntriesModel
 *
 */
class EntriesProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

    Q_PROPERTY(bool onlyUnread READ onlyUnread WRITE setOnlyUnread NOTIFY onlyUnreadChanged)

public:
    explicit EntriesProxyModel(QObject *parent = nullptr);
    ~EntriesProxyModel() override;

    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;

    bool onlyUnread() const;
    void setOnlyUnread(bool onlyUnread);

Q_SIGNALS:
    void onlyUnreadChanged();

private:
    bool m_onlyUnread;
};

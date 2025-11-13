/*
 * SPDX-FileCopyrightText: 2021 Dimitris Kardarakos <dimkard@posteo.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <QQmlEngine>
#include <QSortFilterProxyModel>

/**
 * @brief Filters EntriesModel
 *
 */
class EntriesProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(bool onlyUnread READ onlyUnread WRITE setOnlyUnread NOTIFY onlyUnreadChanged)
    Q_PROPERTY(bool onlyFavorite READ onlyFavorite WRITE setOnlyFavorite NOTIFY onlyFavoriteChanged)

public:
    explicit EntriesProxyModel(QObject *parent = nullptr);
    ~EntriesProxyModel() override;

    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;

    bool onlyUnread() const;
    bool onlyFavorite() const;
    void setOnlyUnread(bool onlyUnread);
    void setOnlyFavorite(bool onlyFavorite);

Q_SIGNALS:
    void onlyUnreadChanged();
    void onlyFavoriteChanged();

private:
    void slotInvalidateFilter();
    bool m_onlyUnread;
    bool m_onlyFavorite;
};

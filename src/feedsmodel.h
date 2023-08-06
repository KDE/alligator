/**
 * SPDX-FileCopyrightText: 2020 Tobias Fella <tobias.fella@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#pragma once

#include <QAbstractListModel>
#include <QHash>
#include <QSqlTableModel>
#include <QUrl>

#include "feed.h"

class FeedsModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit FeedsModel(QObject *parent = nullptr);
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex &parent) const override;
    Q_INVOKABLE void removeFeed(const QString &url);
    Q_INVOKABLE void refreshAll();

private:
    void loadFeed(int index) const;

    mutable QVector<Feed *> m_feeds;
};

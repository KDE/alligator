/**
 * SPDX-FileCopyrightText: 2020 Tobias Fella <tobias.fella@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#pragma once

#include <QAbstractListModel>
#include <QHash>
#include <QObject>
#include <QString>

#include "entry.h"
#include "feed.h"

class EntriesModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(Feed *feed READ feed CONSTANT)

public:
    explicit EntriesModel(Feed *feed = nullptr);
    ~EntriesModel() override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex &parent) const override;

    Feed *feed() const;

private:
    void loadEntry(int index) const;

    Feed *m_feed;
    mutable QHash<int, Entry *> m_entries;
};

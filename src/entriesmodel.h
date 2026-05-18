/**
 * SPDX-FileCopyrightText: 2020 Tobias Fella <tobias.fella@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#pragma once

#include <QAbstractListModel>
#include <QList>
#include <QObject>
#include <QQmlEngine>
#include <QString>

struct Entry {
    QString feedUrl;
    QString id;
    QString title;
    QString content;
    QString feedTitle;
    QString authors;
    QDateTime created;
    QDateTime updated;
    QString link;
    bool read;
    bool favorite;
};

class EntriesModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QString feedUrl READ feedUrl WRITE setFeedUrl NOTIFY feedUrlChanged)
    Q_PROPERTY(QString groupName READ groupName WRITE setGroupName NOTIFY groupNameChanged)

public:
    enum Roles {
        IdRole = Qt::DisplayRole,
        TitleRole,
        ContentRole,
        FeedTitleRole,
        AuthorsRole,
        CreatedRole,
        UpdatedRole,
        LinkRole,
        BaseUrlRole,
        ReadRole,
        FavoritesRole,
    };
    Q_ENUM(Roles);
    explicit EntriesModel(QObject *parent = nullptr);
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex &parent) const override;

    QString feedUrl() const;
    void setFeedUrl(const QString &feedUrl);
    QString groupName() const;
    void setGroupName(const QString &groupName);
    QStringList getFeeds() const;
Q_SIGNALS:
    void feedUrlChanged();
    void groupNameChanged();

private:
    void loadEntries();
    QString m_feedUrl;
    QString m_groupName;
    QList<Entry> m_entries;
};

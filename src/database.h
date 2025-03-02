/**
 * SPDX-FileCopyrightText: 2020 Tobias Fella <tobias.fella@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#pragma once

#include <QObject>
#include <QQmlEngine>
#include <QSqlQuery>

class Database : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

public:
    static Database &instance()
    {
        static Database _instance;
        return _instance;
    }

    static Database *create(QQmlEngine *, QJSEngine *)
    {
        QQmlEngine::setObjectOwnership(&instance(), QQmlEngine::CppOwnership);
        return &instance();
    }
    bool execute(QSqlQuery &query);
    bool execute(const QString &query);
    Q_INVOKABLE void addFeed(const QString &url, const QString &groupName = QString(), const bool markEntriesRead = false);
    Q_INVOKABLE void importFeeds(const QString &path);
    Q_INVOKABLE void exportFeeds(const QString &path);
    Q_INVOKABLE void addFeedGroup(const QString &name, const QString &description, const int isDefault = 0);
    Q_INVOKABLE void removeFeedGroup(const QString &name);
    Q_INVOKABLE void setDefaultGroup(const QString &name);
    Q_INVOKABLE void editFeed(const QString &url, const QString &displayName, const QString &groupName);
    Q_INVOKABLE void setRead(const QString &entryId, bool read);
    Q_INVOKABLE void setFeedRead(const QString &url, bool read);
    Q_INVOKABLE void setAllRead(bool read);
    Q_INVOKABLE void setFavorite(const QString &entryId, bool favorite);

Q_SIGNALS:
    void feedAdded(const QString &url);
    void feedDetailsUpdated(const QString &url, const QString &displayName, const QString &description);
    void feedGroupsUpdated();
    void feedGroupRemoved(const QString &groupName);
    void entryReadChanged(const QString &entryId, bool read);
    void feedReadChanged();
    void entryFavoriteChanged(const QString &entryId, bool favorite);

private:
    bool feedGroupExists(const QString &name);
    void clearFeedGroup(const QString &name);
    QString defaultGroup();

    Database();
    int version();
    bool migrateTo(const int targetVersion);
    bool migrateTo1();
    bool migrateTo2();
    bool migrateTo3();
    void cleanup();
    bool feedExists(const QString &url);
};

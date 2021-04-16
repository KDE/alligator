/**
 * SPDX-FileCopyrightText: 2020 Tobias Fella <fella@posteo.de>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#pragma once

#include <QSqlQuery>

class Database : public QObject
{
    Q_OBJECT

public:
    static Database &instance()
    {
        static Database _instance;
        return _instance;
    }
    bool execute(QSqlQuery &query);
    bool execute(const QString &query);
    Q_INVOKABLE void addFeed(const QString &url, const QString &groupName = QString());
    Q_INVOKABLE void importFeeds(const QString &path);
    Q_INVOKABLE void exportFeeds(const QString &path);
    Q_INVOKABLE void addFeedGroup(const QString &name, const QString &description, const int isDefault = 0);
    Q_INVOKABLE void removeFeedGroup(const QString &name);
    Q_INVOKABLE void setDefaultGroup(const QString &name);
    Q_INVOKABLE void editFeed(const QString &url, const QString &displayName, const QString &groupName);

Q_SIGNALS:
    void feedAdded(const QString &url);
    void feedDetailsUpdated(const QString &url, const QString &displayName, const QString &description);
    void feedGroupsUpdated();
    void feedGroupRemoved(const QString &groupName);
private:
    bool feedGroupExists(const QString &name);
    void clearFeedGroup(const QString &name);
    QString defaultGroup();

    Database();
    int version();
    bool migrateTo(const int targetVersion);
    bool migrateTo1();
    bool migrateTo2();
    void cleanup();
    bool feedExists(const QString &url);
};

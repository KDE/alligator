/**
 * SPDX-FileCopyrightText: 2020 Tobias Fella <tobias.fella@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#pragma once

#include <QNetworkAccessManager>
#include <QObject>
#include <QQmlEngine>
#include <QUrl>
#include <Syndication/Syndication>

class Fetcher : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_PROPERTY(bool refreshing READ refreshing NOTIFY refreshingChanged)

public:
    static Fetcher &instance()
    {
        static Fetcher _instance;
        return _instance;
    }

    static Fetcher *create(QQmlEngine *, QJSEngine *)
    {
        QQmlEngine::setObjectOwnership(&instance(), QQmlEngine::CppOwnership);
        return &instance();
    }
    Q_INVOKABLE void fetch(const QString &url, const bool markEntriesRead = false);
    Q_INVOKABLE void fetchAll();
    Q_INVOKABLE QString image(const QString &url);
    void removeImage(const QString &url);
    Q_INVOKABLE void download(const QString &url);
    QNetworkReply *get(QNetworkRequest &request);

private:
    Fetcher();

    QString filePath(const QString &url);
    void processFeed(Syndication::FeedPtr feed, const QString &url, const bool markEntriesRead = false);
    void processEntry(Syndication::ItemPtr entry, const QString &url, const bool markEntriesRead = false);
    void processAuthor(Syndication::PersonPtr author, const QString &entryId, const QString &url);
    void processEnclosure(Syndication::EnclosurePtr enclosure, Syndication::ItemPtr entry, const QString &feedUrl);
    QString syndicationErrorToString(Syndication::ErrorCode errorCode);

    bool refreshing() const
    {
        return m_fetchCount > 0;
    };
    void setFetchCount(int count);

    int m_fetchCount;

    QNetworkAccessManager *manager;

Q_SIGNALS:
    void startedFetchingFeed(const QString &url);
    void feedUpdated(const QString &url);
    void feedDetailsUpdated(const QString &url,
                            const QString &name,
                            const QString &image,
                            const QString &link,
                            const QString &description,
                            const QDateTime &lastUpdated);
    void error(const QString &url, int errorId, const QString &errorString);
    void imageDownloadFinished(const QString &url);
    void refreshingChanged(bool refreshing);
};

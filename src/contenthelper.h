// SPDX-FileCopyrightText: 2023 Tobias Fella <tobias.fella@kde.org>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <QObject>
#include <QUrl>

class ContentHelper : public QObject
{
    Q_OBJECT

public:
    explicit ContentHelper(QObject *parent = nullptr);

    Q_INVOKABLE void openLink(const QString &link);
    Q_INVOKABLE QString adjustedContent(int width, int fontSize, const QString &content);
};

// SPDX-FileCopyrightText: 2023 Tobias Fella <tobias.fella@kde.org>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "contenthelper.h"

#include <QDebug>
#include <QDesktopServices>
#include <QRegularExpression>

ContentHelper::ContentHelper(QObject *parent)
    : QObject(parent)
{
}

void ContentHelper::openLink(const QString &link, const QString &baseUrl)
{
    QUrl url(link);
    if (link.startsWith(QStringLiteral("//"))) {
        // we a protocol-relative, see https://en.wikipedia.org/wiki/Wikipedia:Protocol-relative_URL
        url.setScheme(QUrl(link).scheme());
    }

    if (url.isRelative()) {
        QUrl articleUrl(baseUrl);
        url.setHost(articleUrl.host());
        url.setScheme(articleUrl.scheme());
    }
    QDesktopServices::openUrl(url);
}

QString ContentHelper::adjustedContent(int width, int fontSize, const QString &content)
{
    QString ret(content);
    QRegularExpression imgRegex(QStringLiteral("<img ((?!width=\"[0-9]+(px)?\").)*(width=\"([0-9]+)(px)?\")?[^>]*>"));

    QRegularExpressionMatchIterator i = imgRegex.globalMatch(ret);
    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();

        QString imgTag(match.captured());
        if (imgTag.contains(QStringLiteral("wp-smiley"))) {
            imgTag.insert(4, QStringLiteral(" width=\"%1\"").arg(fontSize));
        }

        QString widthParameter = match.captured(4);

        if (widthParameter.length() != 0) {
            if (widthParameter.toInt() > width) {
                imgTag.replace(match.captured(3), QStringLiteral("width=\"%1\"").arg(width));
                imgTag.replace(QRegularExpression(QStringLiteral("height=\"([0-9]+)(px)?\"")), QString());
            }
        } else {
            imgTag.insert(4, QStringLiteral(" width=\"%1\"").arg(width));
        }
        ret.replace(match.captured(), imgTag);
    }

    ret.replace(QStringLiteral("<img"), QStringLiteral("<br /> <img"));
    return ret;
}

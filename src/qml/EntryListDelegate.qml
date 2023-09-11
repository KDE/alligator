/**
 * SPDX-FileCopyrightText: 2020 Tobias Fella <tobias.fella@kde.org>
 * SPDX-FileCopyrightText: 2022 Devin Lin <devin@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import QtQuick.Layouts 1.15

import org.kde.kirigami 2.20 as Kirigami
import org.kde.kirigamiaddons.delegates 1.0 as Delegates

import org.kde.alligator 1.0

Delegates.IndicatorItemDelegate {
    id: delegate

    required property var id
    required property string content
    required property string title
    required property string baseUrl
    required property string link
    required property var authors
    required property date updated
    required property bool read

    unread: !read

    property string feedTitle

    onClicked: {
        while(pageStack.depth > 2) {
            pageStack.pop()
        }

        pageStack.push("qrc:/EntryPage.qml", {
            feedTitle : feedTitle,
            entryId: delegate.id,
            content: delegate.content,
            entryTitle: delegate.title,
            baseUrl: delegate.baseUrl,
            link: delegate.link,
        })
    }

    contentItem: ColumnLayout {
        spacing: Kirigami.Units.smallSpacing * 2

        Controls.Label {
            Layout.fillWidth: true
            text: delegate.title
            font.weight: Font.Medium
            wrapMode: Text.Wrap
            elide: Text.ElideRight
            maximumLineCount: 3
        }

        Controls.Label {
            Layout.fillWidth: true
            elide: Text.ElideRight
            font.pointSize: Kirigami.Theme.smallFont.pointSize
            opacity: 0.9
            text: delegate.updated.toLocaleString(Qt.locale(), Locale.ShortFormat) + (delegate.authors.length === 0 ? "" : " " + i18nc("by <author(s)>", "by") + " " + delegate.authors)
        }
    }
}

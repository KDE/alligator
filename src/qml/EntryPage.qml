/**
 * SPDX-FileCopyrightText: 2020 Tobias Fella <fella@posteo.de>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

import QtQuick 2.14
import QtQuick.Controls 2.14 as Controls
import QtQuick.Layouts 1.14

import org.kde.kirigami 2.12 as Kirigami

import org.kde.alligator 1.0

Kirigami.ScrollablePage {
    id: page

    property QtObject entry
    property string feedTitle

    title: feedTitle

    ColumnLayout {
        Kirigami.Heading {
            text: entry.title
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
        }

        Controls.Label {
            text: page.entry.content
            baseUrl: page.entry.baseUrl
            textFormat: Text.RichText
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
            onLinkActivated: page.entry.openLink(link)
            onWidthChanged: text = entry.adjustedContent(width, font.pixelSize)
            font.pointSize: !(Config.articleFontUseSystem) ? Config.articleFontSize : Kirigami.Units.fontMetrics.font.pointSize
        }
    }

    actions.main: Kirigami.Action {
        text: i18n("Open in Browser")
        icon.name: "globe"
        onTriggered: Qt.openUrlExternally(entry.link)
    }

    Component.onDestruction: page.entry.read = true
}

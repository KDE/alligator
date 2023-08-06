/**
 * SPDX-FileCopyrightText: 2020 Tobias Fella <tobias.fella@kde.org>
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

    property string feedTitle
    property string entryTitle
    property string content
    property string entryId
    property string baseUrl
    property string link

    title: feedTitle

    ContentHelper {
        id: contentHelper
    }

    onEntryIdChanged: {
        Database.setRead(page.entryId, true)
    }

    ColumnLayout {
        Kirigami.Heading {
            text: page.entryTitle
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
        }

        Controls.Label {
            text: page.content
            baseUrl: page.baseUrl
            textFormat: Text.RichText
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
            onLinkActivated: contentHelper.openLink(link)
            onWidthChanged: text = contentHelper.adjustedContent(width, font.pixelSize, page.content)
            font.pointSize: !(Config.articleFontUseSystem) ? Config.articleFontSize : Kirigami.Units.fontMetrics.font.pointSize
        }
    }

    actions.main: Kirigami.Action {
        text: i18n("Open in Browser")
        icon.name: "globe"
        onTriggered: Qt.openUrlExternally(page.link)
    }
}

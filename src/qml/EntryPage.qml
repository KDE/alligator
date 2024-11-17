/**
 * SPDX-FileCopyrightText: 2020 Tobias Fella <tobias.fella@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

import QtQuick
import QtQuick.Controls as Controls
import QtQuick.Layouts

import org.kde.kirigami as Kirigami

import org.kde.alligator

Kirigami.ScrollablePage {
    id: page

    property string feedTitle
    property string entryTitle
    property string content
    property string entryId
    property string baseUrl
    property string link
    property bool favorite

    title: feedTitle

    ContentHelper {
        id: contentHelper
    }

    onEntryIdChanged: {
        Database.setRead(page.entryId, true);
    }

    background: Rectangle {
        color: Kirigami.Theme.backgroundColor
        Kirigami.Theme.colorSet: Kirigami.Theme.View
        Kirigami.Theme.inherit: false
    }

    ColumnLayout {
        id: layout

        Kirigami.SelectableLabel {
            text: page.entryTitle
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
            font.pointSize: Kirigami.Theme.defaultFont.pointSize * 1.35
        }

        Kirigami.SelectableLabel {
            id: contentLabel

            text: page.content
            baseUrl: page.baseUrl
            textFormat: Text.RichText
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
            onLinkActivated: link => contentHelper.openLink(link)
            font.pointSize: !(AlligatorSettings.articleFontUseSystem) ? AlligatorSettings.articleFontSize : Kirigami.Theme.defaultFont.pointSize
        }
    }

    onWidthChanged: contentLabel.text = contentHelper.adjustedContent(layout.width, font.pixelSize, page.content)

    actions: [
        Kirigami.Action {
            text: page.favorite ? i18nc("@action:button", "Remove from Favorites") : i18nc("@action:button", "Add to Favorites")
            icon.name: page.favorite ? "bookmarks-bookmarked" : "bookmarks"
            onTriggered: {
                Database.setFavorite(page.entryId, !page.favorite);
                page.favorite = !page.favorite;
            }
        },
        Kirigami.Action {
            text: i18n("Open in Browser")
            icon.name: "globe"
            onTriggered: Qt.openUrlExternally(page.link)
        }
    ]
}

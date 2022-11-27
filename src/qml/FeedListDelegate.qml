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

Kirigami.BasicListItem {

    signal editFeed(var feedObj)

    text: model.feed.displayName || model.feed.name
    icon: model.feed.refreshing ? "view-refresh" : model.feed.image === "" ? "rss" : Fetcher.image(model.feed.image)
    subtitle: i18np("%1 unread entry", "%1 unread entries", model.feed.unreadEntryCount)

    onClicked: {
        lastFeed = model.feed.url
        while(pageStack.depth > 1)
            pageStack.pop()
        pageStack.push("qrc:/EntryListPage.qml", {"feed": model.feed})
    }

    trailing: RowLayout {
        Controls.ToolButton {
            icon.name: "delete"
            display: Controls.AbstractButton.IconOnly
            text: i18nc("'Feed' is an rss feed", "Delete this Feed")
            onClicked: {
                if(pageStack.depth > 1 && model.feed.url === lastFeed)
                    pageStack.pop()
                feedsModel.removeFeed(model.feed.url)
            }
            Controls.ToolTip {
                text: parent.text
            }
        }
        Controls.ToolButton {
            icon.name: "editor"
            display: Controls.AbstractButton.IconOnly
            text: i18nc("'Feed' is an rss feed", "Edit this Feed")
            onClicked: editFeed(model.feed)
            Controls.ToolTip {
                text: parent.text
            }
        }
    }
}

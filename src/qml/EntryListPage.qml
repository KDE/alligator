/**
 * SPDX-FileCopyrightText: 2020 Tobias Fella <fella@posteo.de>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

import QtQuick 2.14
import QtQuick.Controls 2.14 as Controls
import QtQuick.Layouts 1.14

import org.kde.kirigami 2.20 as Kirigami

import org.kde.alligator 1.0

Kirigami.ScrollablePage {
    id: page

    property var feed
    property bool onlyUnread: onlyUnreadAction.checked

    title: feed === undefined ? i18n("All Entries") : feed.displayName || feed.name
    supportsRefreshing: true

    onRefreshingChanged:
        if(refreshing) {
            if (feed === undefined) {
                Fetcher.fetchAll()
            } else {
                feed.refresh()
            }
        }

    Connections {
        target: feed !== undefined ? feed : Fetcher
        function onRefreshingChanged(refreshing) {
            if(!refreshing)
                page.refreshing = refreshing
        }
    }

    contextualActions: [
        Kirigami.Action {
            id: onlyUnreadAction
            text: i18n("Only Unread")
            checkable: true
            checked: true // TODO: store in settings
            onToggled: page.onlyUnread = checked
        },
        Kirigami.Action {
            visible: feed !== undefined
            iconName: "help-about-symbolic"
            text: i18n("Details")
            onTriggered: {
                while(pageStack.depth > 2)
                    pageStack.pop()
                pageStack.push("qrc:/FeedDetailsPage.qml", {"feed": feed})
            }
        }
    ]

    actions.main: Kirigami.Action {
        iconName: "view-refresh"
        text: i18n("Refresh")
        onTriggered: page.refreshing = true
        visible: !Kirigami.Settings.isMobile || entryList.count === 0
    }

    Kirigami.PromptDialog {
        // Show this dialog only if we can not show the error in the placeholder.
        // This is eg. the case if the feed worked so far, but refreshing fails for some reason
        visible: entryList.count !== 0 && feed !== undefined && feed.errorId !== 0
        title: i18n("Error")
        subtitle: i18n("Error (%1): %2", feed.errorId, feed.errorString)
    }

    Kirigami.PlaceholderMessage {
        visible: entryList.count === 0

        width: Kirigami.Units.gridUnit * 20
        anchors.centerIn: parent

        text: feed === undefined || feed.errorId === 0 ? i18n("No entries available") : i18n("Error (%1): %2", feed.errorId, feed.errorString)
        icon.name: feed === undefined || feed.errorId === 0 ? "" : "data-error"
    }

    ListView {
        id: entryList
        visible: count !== 0
        model: proxyModel

        delegate: EntryListDelegate { feedTitle : feed === undefined ? i18n("All Entries") : feed.displayName || feed.name }
    }

    EntriesProxyModel {
        id: proxyModel
        onlyUnread: page.onlyUnread
        sourceModel: page.feed === undefined ? allEntriesModel : page.feed.entries
    }

    EntriesModel {
        id: allEntriesModel
    }
}

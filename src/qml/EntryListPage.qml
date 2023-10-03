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
import org.kde.alligator.config

Kirigami.ScrollablePage {
    id: page

    property var feed
    property bool onlyUnread: false

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

    actions: [
        Kirigami.Action {
            icon.name: "view-refresh"
            text: i18n("Refresh")
            onTriggered: page.refreshing = true
            visible: !Kirigami.Settings.isMobile
        },
        Kirigami.Action {
            visible: feed !== undefined
            icon.name: "help-about-symbolic"
            text: i18n("Details")
            onTriggered: {
                while(pageStack.depth > 2)
                    pageStack.pop()
                pageStack.push("qrc:/qrc/FeedDetailsPage.qml", {feed: feed})
            }
        }
    ]

    titleDelegate: RowLayout {
        spacing: Kirigami.Units.smallSpacing
        Controls.ToolButton {
            visible: !applicationWindow().globalDrawer.modal
            text: applicationWindow().sidebarCollapsed ? i18n("Expand sidebar") : i18n("Collapse sidebar")
            icon.name: applicationWindow().sidebarCollapsed ? "sidebar-expand" : "sidebar-collapse"
            display: Controls.ToolButton.IconOnly
            onClicked: {
                applicationWindow().sidebarCollapsed = !applicationWindow().sidebarCollapsed
                Config.sidebarCollapsed = applicationWindow().sidebarCollapsed
            }

        }

        Kirigami.Heading {
            Layout.fillWidth: true
            text: feed === undefined ? i18n("All Entries") : feed.displayName || feed.name
        }
    }

    Kirigami.PromptDialog {
        // Show this dialog only if we can not show the error in the placeholder.
        // This is eg. the case if the feed worked so far, but refreshing fails for some reason
        visible: entryList.count !== 0 && feed !== undefined && feed.errorId !== 0
        title: i18n("Error")
        subtitle: i18n("Error (%1): %2", feed ? feed.errorId : "", feed ? feed.errorString : "")
    }

    Kirigami.PlaceholderMessage {
        visible: entryList.count === 0

        width: Kirigami.Units.gridUnit * 20
        anchors.centerIn: parent

        text: if(feed === undefined || feed.errorId === 0) {
            root.onlyUnread ? i18n("No unread entries available") : i18n("No entries available")
        } else {
            i18n("Error (%1): %2", feed.errorId, feed.errorString)
        }
        icon.name: feed === undefined || feed.errorId === 0 ? "" : "data-error"
    }

    ListView {
        id: entryList
        visible: count !== 0
        model: proxyModel

        // stop list highlight
        currentIndex: -1

        delegate: EntryListDelegate { feedTitle: (feed === undefined ? "" : feed.displayName) || "" }
    }

    EntriesProxyModel {
        id: proxyModel
        onlyUnread: page.onlyUnread
        sourceModel: entriesModel
    }

    EntriesModel {
        id: entriesModel
        feedUrl: page.feed ? page.feed.url : ""
    }

    footer: ColumnLayout {
        spacing: 0

        Kirigami.Separator { Layout.fillWidth: true }

        Kirigami.NavigationTabBar {
            Layout.fillWidth: true
            shadow: false
            actions: [
                Kirigami.Action {
                    icon.name: "mail-read"
                    text: i18n("All")
                    checked: !page.onlyUnread
                    onTriggered: page.onlyUnread = false
                },
                Kirigami.Action {
                    icon.name: "mail-mark-unread"
                    text: i18n("Unread")
                    checked: page.onlyUnread
                    onTriggered: page.onlyUnread = true;
                }
            ]
        }
    }
}

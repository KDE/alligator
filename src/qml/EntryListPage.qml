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

    property Feed feed
    property bool onlyUnread: false

    supportsRefreshing: true

    onRefreshingChanged: {
        if (refreshing) {
            if (feed === null) {
                Fetcher.fetchAll();
            } else {
                feed.refresh();
            }
        }
    }

    Connections {
        target: page.feed !== null ? page.feed : Fetcher
        function onRefreshingChanged(refreshing) {
            if (!refreshing)
                page.refreshing = refreshing;
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
            visible: page.feed !== null
            icon.name: "help-about-symbolic"
            text: i18n("Details")
            onTriggered: {
                while (pageStack.depth >= 2)
                    pageStack.pop();

                pageStack.push(Qt.resolvedUrl("FeedDetailsPage.qml"), {
                    feed: feed
                });
            }
        }
    ]

    titleDelegate: RowLayout {
        spacing: Kirigami.Units.smallSpacing
        Controls.ToolButton {
            visible: !applicationWindow().globalDrawer.modal
            text: AlligatorSettings.sidebarCollapsed ? i18n("Expand sidebar") : i18n("Collapse sidebar")
            icon.name: AlligatorSettings.sidebarCollapsed ? "sidebar-expand" : "sidebar-collapse"
            display: Controls.ToolButton.IconOnly
            onClicked: {
                AlligatorSettings.sidebarCollapsed = !AlligatorSettings.sidebarCollapsed;
            }
        }

        Kirigami.Heading {
            Layout.fillWidth: true
            text: page.feed === null ? i18n("All Entries") : page.feed.displayName || page.feed.name
        }
    }

    Kirigami.PromptDialog {
        // Show this dialog only if we can not show the error in the placeholder.
        // This is eg. the case if the feed worked so far, but refreshing fails for some reason
        visible: entryList.count !== 0 && page.feed !== null && page.feed.errorId !== 0
        title: i18n("Error")
        subtitle: i18n("Error (%1): %2", page.feed ? page.feed.errorId : "", page.feed ? page.feed.errorString : "")
    }

    Kirigami.PlaceholderMessage {
        visible: entryList.count === 0

        width: Kirigami.Units.gridUnit * 20
        anchors.centerIn: parent

        text: {
            if (page.feed === null || page.feed.errorId === 0) {
                root.onlyUnread ? i18n("No unread entries available") : i18n("No entries available");
            } else {
                i18n("Error (%1): %2", feed.errorId, feed.errorString);
            }
        }
        icon.name: page.feed === null || page.feed.errorId === 0 ? "data-error" : ""
    }

    ListView {
        id: entryList
        visible: count !== 0
        model: proxyModel

        // stop list highlight
        currentIndex: -1

        delegate: EntryListDelegate {
            feedTitle: (page.feed === null ? "" : feed.displayName) || ""
        }
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

        Kirigami.NavigationTabBar {
            Layout.fillWidth: true
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
                    onTriggered: page.onlyUnread = true
                }
            ]
        }
    }
}

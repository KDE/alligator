/**
 * SPDX-FileCopyrightText: 2020 Tobias Fella <tobias.fella@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

import QtQuick
import QtQuick.Controls as Controls
import Qt.labs.platform
import QtQuick.Layouts

import org.kde.kirigami as Kirigami

import org.kde.alligator

Kirigami.ScrollablePage {
    id: root
    title: i18nc("'Feeds' as in 'RSS Feeds'", "Manage Feeds")

    supportsRefreshing: true
    onRefreshingChanged:
        if(refreshing)  {
            Fetcher.fetchAll()
            refreshing = false
        }

    contextualActions: [
        Kirigami.Action {
            text: i18n("Refresh All Feeds")
            iconName: "view-refresh"
            onTriggered: refreshing = true
            visible: !Kirigami.Settings.isMobile
        },
        Kirigami.Action {
            text: i18n("Manage Feed Groups")
            iconName: "edit-group"
            onTriggered: applicationWindow().pageStack.layers.push(groupsList)
        },
        Kirigami.Action {
            text: i18n("Import Feeds…")
            iconName: "document-import"
            onTriggered: importDialog.open()
        },
        Kirigami.Action {
            text: i18n("Export Feeds…")
            iconName: "document-export"
            onTriggered: exportDialog.open()
        }
    ]

    Component {
        id: groupsList
        GroupsListPage {
            feedGroupsModel: FeedGroupsModel {
                id: groupsModel
            }
        }
    }

    AddFeedDialog {
        id: addDialog
    }

    EditFeedDialog {
        id: editDialog
    }

    actions.main: Kirigami.Action {
        text: i18n("Add Feed…")
        iconName: "list-add"
        onTriggered: {
            addDialog.open()
        }
    }

    Kirigami.PlaceholderMessage {
        visible: feedList.count === 0

        width: Kirigami.Units.gridUnit * 20
        icon.name: "rss"
        anchors.centerIn: parent

        text: i18n("No feeds added yet")
    }

    ListView {
        id: feedList
        currentIndex: -1

        anchors.fill: parent

        model: FeedsProxyModel {
            id: proxyModel
            groupName: ""
            sourceModel: feedsModel
        }

        delegate: FeedListDelegate {
            onEditFeed: {
                editDialog.feed = feedObj
                editDialog.open()
            }
        }

        FeedsModel {
            id: feedsModel
        }

        section {
            property: "feed.groupName"
            criteria: ViewSection.FullString
            delegate: Kirigami.ListSectionHeader {
                label: section
            }
        }
    }

    FileDialog {
        id: importDialog
        title: i18n("Import Feeds")
        folder: StandardPaths.writableLocation(StandardPaths.HomeLocation)
        nameFilters: [i18n("All Files (*)"), i18n("XML Files (*.xml)"), i18n("OPML Files (*.opml)")]
        onAccepted: Database.importFeeds(file)
    }

    FileDialog {
        id: exportDialog
        title: i18n("Export Feeds")
        folder: StandardPaths.writableLocation(StandardPaths.HomeLocation)
        nameFilters: [i18n("All Files")]
        onAccepted: Database.exportFeeds(file)
        fileMode: FileDialog.SaveFile
    }

    Component.onCompleted: Fetcher.fetchAll()
}

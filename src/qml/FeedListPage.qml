/**
 * SPDX-FileCopyrightText: 2020 Tobias Fella <fella@posteo.de>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

import QtQuick 2.14
import QtQuick.Controls 2.14 as Controls
import Qt.labs.platform 1.1
import QtQuick.Layouts 1.14

import org.kde.kirigami 2.12 as Kirigami

import org.kde.alligator 1.0

Kirigami.ScrollablePage {
    id: root
    title: groupFilter ? groupFilter : "Alligator"

    property var lastFeed: ""
    property alias groupFilter: proxyModel.groupName

    supportsRefreshing: true
    onRefreshingChanged:
        if(refreshing)  {
            Fetcher.fetchAll()
            refreshing = false
        }

    contextualActions: [
        Kirigami.Action {
            text: i18n("Refresh All Feeds")
            icon.name: "view-refresh"
            onTriggered: refreshing = true
            visible: !Kirigami.Settings.isMobile
        },
        Kirigami.Action {
            text: i18n("Manage Feed Groups")
            icon.name: "edit-group"
            onTriggered: applicationWindow().pageStack.layers.push(groupsList)
        },
        Kirigami.Action {
            text: i18n("Import Feeds...")
            icon.name: "document-import"
            visible: root.groupFilter === ""
            onTriggered: importDialog.open()
        },
        Kirigami.Action {
            text: i18n("Export Feeds...")
            icon.name: "document-export"
            visible: root.groupFilter === ""
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
        groupName: root.groupFilter
    }

    EditFeedDialog {
        id: editDialog
    }

    actions.main: Kirigami.Action {
        text: i18n("Add Feed…")
        icon.name: "list-add"
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
            onEditFeed: feedObj => {
                editDialog.feed = feedObj
                editDialog.open()
            }
        }

        FeedsModel {
            id: feedsModel
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

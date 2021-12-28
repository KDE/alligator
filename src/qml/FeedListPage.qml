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
            text: i18n("Refresh all feeds")
            iconName: "view-refresh"
            onTriggered: refreshing = true
            visible: !Kirigami.Settings.isMobile
        },
        Kirigami.Action {
            text: i18n("Import Feeds...")
            iconName: "document-import"
            visible: root.groupFilter === ""
            onTriggered: importDialog.open()
        },
        Kirigami.Action {
            text: i18n("Export Feeds...")
            iconName: "document-export"
            visible: root.groupFilter === ""
            onTriggered: exportDialog.open()
        }
    ]

    AddFeedSheet {
        id: addSheet
        groupName: root.groupFilter
    }

    EditFeedSheet {
        id: editSheet
    }

    actions.main: Kirigami.Action {
        text: i18n("Add feed")
        iconName: "list-add"
        onTriggered: {
            addSheet.open()
        }
    }

    Kirigami.PlaceholderMessage {
        visible: feedList.count === 0

        width: Kirigami.Units.gridUnit * 20
        icon.name: "rss"
        anchors.centerIn: parent

        text: i18n("No Feeds added yet")
    }

    ListView {
        id: feedList

        anchors.fill: parent

        model: FeedsProxyModel {
            id: proxyModel
            groupName: ""
            sourceModel: feedsModel
        }

        delegate: FeedListDelegate {
            onEditFeed: {
                editSheet.feed = feedObj
                editSheet.open()
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

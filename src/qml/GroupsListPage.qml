/*
* SPDX-FileCopyrightText: 2021 Dimitris Kardarakos <dimkard@posteo.net>
*
* SPDX-License-Identifier: GPL-3.0-or-later
*/

import QtQuick
import QtQuick.Controls as Controls
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.alligator

Kirigami.ScrollablePage {
    id: root

    property var feedGroupsModel

    title: i18n("Groups")

    globalToolBarStyle: Kirigami.ApplicationHeaderStyle.ToolBar
    actions: [
        Kirigami.Action {
            icon.name: "list-add"
            text: i18n("Add Group…")

            onTriggered: feedGroupDialog.open()
        }
    ]

    ListView {
        id: groupsListView
        model: root.feedGroupsModel

        currentIndex: -1

        delegate: Kirigami.SwipeListItem {
            font.bold: model.isDefault
            activeBackgroundColor: "transparent"
            activeTextColor: Kirigami.Theme.textColor
            contentItem: ColumnLayout {
                Controls.Label {
                    text: model.name
                    wrapMode: Text.WordWrap
                }

                Controls.Label {
                    text: model.description
                    font: Kirigami.Theme.smallFont
                    wrapMode: Text.WordWrap
                }
            }

            actions: [
                Kirigami.Action {
                    text: i18n("Remove")
                    icon.name: "delete"
                    enabled: !model.isDefault

                    onTriggered: Database.removeFeedGroup(model.name)
                },
                Kirigami.Action {
                    icon.name: "emblem-default-symbolic"
                    text: i18n("Set as Default")
                    enabled: !model.isDefault
                    onTriggered: Database.setDefaultGroup(model.name)
                }
            ]
        }

        Kirigami.PlaceholderMessage {
            anchors.centerIn: parent
            text: i18n("No groups created yet")
            visible: groupsListView.count === 0

            helpfulAction: Kirigami.Action {
                icon.name: "list-add"
                text: i18n("Add Group")
                onTriggered: feedGroupDialog.open()
            }
        }
    }

    FeedGroupDialog {
        id: feedGroupDialog

        groupsModel: root.feedGroupsModel
    }
}

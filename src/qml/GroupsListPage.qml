/*
* SPDX-FileCopyrightText: 2021 Dimitris Kardarakos <dimkard@posteo.net>
*
* SPDX-License-Identifier: GPL-3.0-or-later
*/

import QtQuick 2.14
import QtQuick.Controls 2.14 as Controls
import QtQuick.Layouts 1.14
import org.kde.kirigami 2.12 as Kirigami
import org.kde.alligator 1.0 as Alligator

Kirigami.ScrollablePage {
    id: root

    property var feedGroupsModel

    title: i18n("Groups")

    actions.main: Kirigami.Action {
        iconName: "list-add"
        text: i18n("Add Group…")

        onTriggered: feedGroupDialog.open()
    }

    ListView {
        id: groupsListView
        model: feedGroupsModel

        delegate: Kirigami.SwipeListItem {
            font.bold: model.isDefault
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
                    iconName: "delete"
                    enabled: !model.isDefault

                    onTriggered: Alligator.Database.removeFeedGroup(model.name)
                },
                Kirigami.Action {
                    icon.name: "emblem-default-symbolic"
                    text: i18n("Set as Default")
                    enabled: !model.isDefault
                    onTriggered: Alligator.Database.setDefaultGroup(model.name)
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

        groupsModel: feedGroupsModel
    }
}

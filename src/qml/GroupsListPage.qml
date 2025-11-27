/*
* SPDX-FileCopyrightText: 2021 Dimitris Kardarakos <dimkard@posteo.net>
*
* SPDX-License-Identifier: GPL-3.0-or-later
*/

import QtQuick
import QtQuick.Controls as Controls
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.delegates as Delegates
import org.kde.alligator

Kirigami.ScrollablePage {
    id: root

    property var feedGroupsModel

    title: i18n("Groups")

    globalToolBarStyle: Kirigami.ApplicationHeaderStyle.ToolBar
    actions: [
        Kirigami.Action {
            text: i18nc("@action:intoolbar", "Add Group…")
            icon.name: "list-add"

            onTriggered: feedGroupDialog.open()
        }
    ]

    ListView {
        id: groupsListView
        model: root.feedGroupsModel
        currentIndex: -1

        delegate: Delegates.RoundedItemDelegate {
            id: delegate

            required property var model

            width: ListView.view.width

            contentItem: RowLayout {
                Kirigami.TitleSubtitle {
                    title: delegate.model.name
                    font.bold: delegate.model.isDefault
                    subtitle: delegate.model.description
                }

                RowLayout {
                    Layout.alignment: Qt.AlignRight

                    Controls.ToolButton {
                        text: i18nc("@action:button", "Set as Default")
                        icon.name: "emblem-default-symbolic"
                        visible: !delegate.model.isDefault
                        display: Controls.Button.IconOnly

                        onClicked: Database.setDefaultGroup(delegate.model.name)

                        Controls.ToolTip.text: text
                        Controls.ToolTip.visible: hovered && enabled && visible
                        Controls.ToolTip.delay: Kirigami.Units.toolTipDelay
                    }

                    Controls.ToolButton {
                        text: i18nc("@action:button", "Remove")
                        icon.name: "delete"
                        enabled: !delegate.model.isDefault
                        display: Controls.Button.IconOnly

                        onClicked: Database.removeFeedGroup(delegate.model.name)

                        Controls.ToolTip.text: text
                        Controls.ToolTip.visible: hovered && enabled && visible
                        Controls.ToolTip.delay: Kirigami.Units.toolTipDelay
                    }
                }
            }
        }

        Kirigami.PlaceholderMessage {
            text: i18nc("@info:placeholder", "No groups created yet")
            visible: groupsListView.count === 0
            anchors.centerIn: parent

            helpfulAction: Kirigami.Action {
                text: i18nc("@action:button", "Add Group…")
                icon.name: "list-add"

                onTriggered: feedGroupDialog.open()
            }
        }
    }

    FeedGroupDialog {
        id: feedGroupDialog

        groupsModel: root.feedGroupsModel
    }
}

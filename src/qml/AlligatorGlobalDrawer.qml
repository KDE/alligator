/**
 * SPDX-FileCopyrightText: 2021 Dimitris Kardarakos <dimkard@posteo.net>
 * SPDX-FileCopyrightText: 2020 Tobias Fella <fella@posteo.de>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

import QtQuick 2.14
import QtQuick.Controls 2.14 as Controls
import org.kde.kirigami 2.12 as Kirigami
import org.kde.alligator 1.0 as Alligator

Kirigami.GlobalDrawer {
    id: root

    property var entriesPage
    property var feedsPage

    isMenu: true
    actions: [
        Kirigami.Action {
            text: i18n("All Entries")
            iconName: "rss"
            onTriggered: {
                pageStack.layers.clear()
                pageStack.clear()
                pageStack.push(root.entriesPage)
            }
        },

        Kirigami.Action {
            text: i18n("All Feeds")
            iconName: "rss"
            onTriggered: {
                pageStack.layers.clear()
                pageStack.clear()
                pageStack.push(root.feedsPage, {groupFilter: ""})
            }
        },

        Kirigami.Action {
            id: feedGroups
            iconName: "edit-group"
            text: i18n("Feed Groups")
            Kirigami.Action {
                id: configureGroupsAction

                text: i18n("Configure Groups")
                iconName: "settings-configure"
                onTriggered: {
                    pageStack.clear()
                    pageStack.insertPage(0, root.feedsPage, {groupFilter: ""})
                    pageStack.layers.clear()
                    pageStack.layers.push(groupsList)
                }
            }
        },
        Kirigami.Action {
            text: i18n("Settings")
            iconName: "settings-configure"
            onTriggered: pageStack.layers.push("qrc:/SettingsPage.qml")
            enabled: pageStack.layers.currentItem.title !== i18n("Settings")
        },
        Kirigami.Action {
            text: i18n("About")
            iconName: "help-about-symbolic"
            onTriggered: pageStack.layers.push(aboutPage)
            enabled: pageStack.layers.currentItem.title !== i18n("About")
        }
    ]

    Instantiator {
        model: groupsModel

        delegate: Kirigami.Action {
            text: model.name

            onTriggered: {
                pageStack.layers.clear()
                pageStack.clear()
                pageStack.push(root.feedsPage, {groupFilter: text})
            }
        }

        onObjectAdded: {
            feedGroups.children.push(object)
        }

        onObjectRemoved: {
            feedGroups.children = [configureGroupsAction];
        }
    }

    Component {
        id: groupsList
        GroupsListPage {
            feedGroupsModel: groupsModel
        }
    }

    Alligator.FeedGroupsModel {
        id: groupsModel
    }
}

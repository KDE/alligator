/**
 * SPDX-FileCopyrightText: 2022 Devin Lin <devin@kde.org>
 * SPDX-FileCopyrightText: 2021 Dimitris Kardarakos <dimkard@posteo.net>
 * SPDX-FileCopyrightText: 2020 Tobias Fella <tobias.fella@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.delegates as Delegates

import org.kde.alligator

Kirigami.GlobalDrawer {
    id: root

    property var entriesPage
    property var feedsPage

    modal: applicationWindow().width < Kirigami.Units.gridUnit * 40

    height: applicationWindow().height
    width: applicationWindow().sidebarCollapsed ? Kirigami.Units.iconSizes.medium : Kirigami.Units.gridUnit * 15

    Behavior on width {
        NumberAnimation {
            easing.type: Easing.InOutExpo
            duration: Kirigami.Units.veryLongDuration
        }
    }

    leftPadding: 0
    rightPadding: 0
    topPadding: 0
    bottomPadding: 0

    Kirigami.Theme.colorSet: Kirigami.Theme.View
    Kirigami.Theme.inherit: false

    contentItem: ColumnLayout {
        spacing: 0

        Controls.ToolBar {
            Layout.fillWidth: true
            implicitHeight: applicationWindow().pageStack.globalToolBar.preferredHeight

            Item {
                anchors.fill: parent
                Kirigami.Heading {
                    level: 1
                    text: i18n("Alligator")
                    anchors.left: parent.left
                    anchors.leftMargin: Kirigami.Units.largeSpacing + Kirigami.Units.smallSpacing
                    anchors.verticalCenter: parent.verticalCenter
                    opacity: applicationWindow().sidebarCollapsed ? 0 : 1

                    Behavior on opacity {
                        NumberAnimation {
                            easing.type: Easing.InOutQuad
                            duration: Kirigami.Units.shortDuration
                        }
                    }
                }
            }
        }

        ListView {
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.topMargin: Math.round(Kirigami.Units.smallSpacing / 2)
            Layout.bottomMargin: Math.round(Kirigami.Units.smallSpacing / 2)

            clip: true

            // stop list highlight
            currentIndex: -1
            onCurrentIndexChanged: {
                if (currentIndex != -1) {
                    currentIndex = -1;
                }
            }

            header: Delegates.RoundedItemDelegate {
                id: allFeedsItem
                text: applicationWindow().sidebarCollapsed ? "" : i18n("All Feeds")
                icon.name: "rss"
                onClicked: {
                    pageStack.clear()
                    pageStack.push(root.entriesPage)
                }
                Controls.ToolTip.visible: hovered && applicationWindow().sidebarCollapsed
                Controls.ToolTip.text: i18n("All Feeds")
                Controls.ToolTip.delay: Kirigami.Units.toolTipDelay
            }

            section {
                property: "feed.groupName"
                criteria: ViewSection.FullString
                delegate: Kirigami.ListSectionHeader {
                    label: section
                    opacity: applicationWindow().sidebarCollapsed ? 0 : 1
                    width: parent.width

                    Behavior on opacity {
                        NumberAnimation {
                            easing.type: Easing.InOutQuad
                            duration: Kirigami.Units.shortDuration
                        }
                    }
                }
            }

            model: FeedsProxyModel {
                id: proxyModel
                groupName: ""
                sourceModel: feedsModel
            }

            FeedsModel {
                id: feedsModel
            }

            delegate: Delegates.RoundedItemDelegate {
                required property var feed

                text: feed.displayName || feed.name
                icon.name: feed.refreshing ? "view-refresh" : feed.image === "" ? "rss" : Fetcher.image(feed.image)
                onClicked: {
                    pageStack.layers.clear();
                    pageStack.clear();
                    pageStack.push("qrc:/qml/EntryListPage.qml", {feed: feed})
                }
                Controls.ToolTip.visible: hovered && applicationWindow().sidebarCollapsed
                Controls.ToolTip.text: feed.displayName || feed.name
                Controls.ToolTip.delay: Kirigami.Units.toolTipDelay
            }
        }

        Kirigami.Separator {
            Layout.fillWidth: true
            Layout.leftMargin: Kirigami.Units.smallSpacing
            Layout.rightMargin: Kirigami.Units.smallSpacing
        }

        Delegates.RoundedItemDelegate {
            Layout.fillWidth: true
            Layout.topMargin: Math.round(Kirigami.Units.smallSpacing / 2)
            text: applicationWindow().sidebarCollapsed ? "" : i18n("Settings")
            icon.name: "settings-configure"
            onClicked: {
                pageStack.layers.clear();
                pageStack.clear();
                pageStack.push("qrc:/qml/SettingsPage.qml");
            }
        }

        Delegates.RoundedItemDelegate {
            Layout.fillWidth: true
            text: applicationWindow().sidebarCollapsed ? "" : i18n("Manage Feeds")
            icon.name: "feed-subscribe"
            onClicked: {
                pageStack.layers.clear();
                pageStack.clear();
                pageStack.push(root.feedsPage, {});
            }
        }

        Delegates.RoundedItemDelegate {
            Layout.fillWidth: true
            Layout.bottomMargin: Math.round(Kirigami.Units.smallSpacing / 2)
            text: applicationWindow().sidebarCollapsed ? "" : i18n("About")
            icon.name: "documentinfo"
            onClicked: {
                pageStack.layers.clear();
                pageStack.clear();
                pageStack.push(aboutPage);
            }
        }
    }
}

/**
 * SPDX-FileCopyrightText: 2022 Devin Lin <devin@kde.org>
 * SPDX-FileCopyrightText: 2021 Dimitris Kardarakos <dimkard@posteo.net>
 * SPDX-FileCopyrightText: 2020 Tobias Fella <tobias.fella@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

import QtQuick 2.14
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.14 as Controls

import org.kde.kirigami 2.12 as Kirigami
import org.kde.alligator 1.0 as Alligator

Kirigami.GlobalDrawer {
    id: root

    property var entriesPage
    property var feedsPage

    // updated by allFeedsItem
    property real iconSizing: 0
    property real listItemPadding: 0

    modal: applicationWindow().width < Kirigami.Units.gridUnit * 40

    height: applicationWindow().height
    width: applicationWindow().sidebarCollapsed ? (iconSizing + listItemPadding * 2) : Kirigami.Units.gridUnit * 15

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
            clip: true

            // stop list highlight
            currentIndex: -1
            onCurrentIndexChanged: {
                if (currentIndex != -1) {
                    currentIndex = -1;
                }
            }

            header: Kirigami.BasicListItem {
                id: allFeedsItem
                text: i18n("All Feeds")
                icon: "rss"
                onIconSizeChanged: root.iconSizing = iconSize
                onLeadingPaddingChanged: root.listItemPadding = leadingPadding
                onClicked: {
                    pageStack.clear()
                    pageStack.push(root.entriesPage)
                }
            }

            section.property: "feed.groupName"
            section.criteria: ViewSection.FullString
            section.delegate: Kirigami.ListSectionHeader {
                label: section
                opacity: applicationWindow().sidebarCollapsed ? 0 : 1

                Behavior on opacity {
                    NumberAnimation {
                        easing.type: Easing.InOutQuad
                        duration: Kirigami.Units.shortDuration
                    }
                }
            }

            model: Alligator.FeedsProxyModel {
                id: proxyModel
                groupName: ""
                sourceModel: feedsModel
            }

            Alligator.FeedsModel {
                id: feedsModel
            }

            delegate: Kirigami.BasicListItem {
                text: model.feed.displayName || model.feed.name
                icon: model.feed.refreshing ? "view-refresh" : model.feed.image === "" ? "rss" : Alligator.Fetcher.image(model.feed.image)
                onClicked: {
                    pageStack.layers.clear();
                    pageStack.clear();
                    pageStack.push("qrc:/EntryListPage.qml", {feed: model.feed})
                }
            }
        }

        Kirigami.Separator { Layout.fillWidth: true }

        Kirigami.BasicListItem {
            Layout.fillWidth: true
            text: i18n("Settings")
            icon: "settings-configure"
            onClicked: {
                pageStack.layers.clear();
                pageStack.clear();
                pageStack.push("qrc:/SettingsPage.qml");
            }
        }

        Kirigami.BasicListItem {
            Layout.fillWidth: true
            text: i18n("Manage Feeds")
            icon: "feed-subscribe"
            onClicked: {
                pageStack.layers.clear();
                pageStack.clear();
                pageStack.push(root.feedsPage, {});
            }
        }

        Kirigami.BasicListItem {
            Layout.fillWidth: true
            text: i18n("About")
            icon: "documentinfo"
            onClicked: {
                pageStack.layers.clear();
                pageStack.clear();
                pageStack.push(aboutPage);
            }
        }
    }
}

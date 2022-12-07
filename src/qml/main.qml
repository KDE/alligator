/**
 * SPDX-FileCopyrightText: 2020 Tobias Fella <fella@posteo.de>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

import QtQuick 2.14
import QtQuick.Controls 2.14 as Controls

import org.kde.kirigami 2.12 as Kirigami
import org.kde.kirigamiaddons.labs.mobileform 0.1 as MobileForm
import org.kde.alligator 1.0

Kirigami.ApplicationWindow {
    id: root

    title: "Alligator"
    width: 1200
    height: 700

    property bool sidebarCollapsed: false

    pageStack.initialPage: entryList
    pageStack.globalToolBar.style: Kirigami.ApplicationHeaderStyle.ToolBar
    pageStack.globalToolBar.showNavigationButtons: Kirigami.ApplicationHeaderStyle.ShowBackButton

    globalDrawer: AlligatorGlobalDrawer {
        entriesPage: entryList
        feedsPage: feedList
    }

    Component {
        id: aboutPage
        MobileForm.AboutPage {
            aboutData: About
        }
    }

    contextDrawer: Kirigami.ContextDrawer {
        id: contextDrawer
    }

    Component {
        id: entryList

        EntryListPage {}
    }

    Component {
        id: feedList

        FeedListPage  {}
    }
}

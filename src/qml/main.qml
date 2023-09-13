/**
 * SPDX-FileCopyrightText: 2020 Tobias Fella <tobias.fella@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

import QtQuick
import QtQuick.Controls as Controls

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard
import org.kde.alligator

Kirigami.ApplicationWindow {
    id: root

    title: "Alligator"

    property bool sidebarCollapsed: Config.sidebarCollapsed

    pageStack.initialPage: entryList
    pageStack.globalToolBar.style: Kirigami.ApplicationHeaderStyle.ToolBar
    pageStack.globalToolBar.showNavigationButtons: Kirigami.ApplicationHeaderStyle.ShowBackButton

    globalDrawer: AlligatorGlobalDrawer {
        entriesPage: entryList
        feedsPage: feedList
    }

    Component {
        id: aboutPage
        FormCard.AboutPage {
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

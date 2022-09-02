/**
 * SPDX-FileCopyrightText: 2020 Tobias Fella <fella@posteo.de>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

import QtQuick 2.14
import QtQuick.Controls 2.14 as Controls

import org.kde.kirigami 2.12 as Kirigami
import org.kde.alligator 1.0 as Alligator

Kirigami.ApplicationWindow {
    id: root

    title: "Alligator"

    pageStack.initialPage: entryList

    globalDrawer: AlligatorGlobalDrawer {
        entriesPage: entryList
        feedsPage: feedList
    }

    Component {
        id: aboutPage
        Kirigami.AboutPage {
            aboutData: Alligator.AboutType.aboutData
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

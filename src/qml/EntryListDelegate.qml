/**
 * SPDX-FileCopyrightText: 2020 Tobias Fella <fella@posteo.de>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

import QtQuick 2.14
import QtQuick.Controls 2.14 as Controls
import QtQuick.Layouts 1.14

import org.kde.kirigami 2.12 as Kirigami

import org.kde.alligator 1.0

Kirigami.SwipeListItem {

    property string feedTitle

    leftPadding: 0
    rightPadding: 0

    contentItem: Kirigami.BasicListItem {
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        text: model.entry.title
        subtitle: model.entry.updated.toLocaleString(Qt.locale(), Locale.ShortFormat) + (model.entry.authors.length === 0 ? "" : " " + i18nc("by <author(s)>", "by") + " " + model.entry.authors[0].name)
        reserveSpaceForIcon: false
        bold: !model.entry.read

        onClicked: {
            while(pageStack.depth > 2)
                pageStack.pop()
            pageStack.push("qrc:/EntryPage.qml", {"entry": model.entry, "feedTitle" : feedTitle})
        }
    }

    actions: [
        Kirigami.Action {
            icon.name: model.entry.read ? "mail-mark-unread" : "mail-mark-read"
            tooltip: model.entry.read ? i18n("Mark as unread") : i18n("Mark as read")
            onTriggered: model.entry.read = !model.entry.read
        }
    ]
}

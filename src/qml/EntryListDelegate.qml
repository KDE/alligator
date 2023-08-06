/**
 * SPDX-FileCopyrightText: 2020 Tobias Fella <tobias.fella@kde.org>
 * SPDX-FileCopyrightText: 2022 Devin Lin <devin@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

import QtQuick 2.14
import QtQuick.Controls 2.14 as Controls
import QtQuick.Layouts 1.14

import org.kde.kirigami 2.12 as Kirigami

import org.kde.alligator 1.0

Kirigami.AbstractListItem {

    property string feedTitle

    onClicked: {
        while(pageStack.depth > 2) {
            pageStack.pop()
        }
        pageStack.push("qrc:/EntryPage.qml", {"entry": model.entry, "feedTitle" : feedTitle})
    }

    separatorVisible: true
    activeBackgroundColor: Qt.rgba(Kirigami.Theme.highlightColor.r, Kirigami.Theme.highlightColor.g, Kirigami.Theme.highlightColor.b, 0.5)
    topPadding: Kirigami.Units.largeSpacing
    bottomPadding: Kirigami.Units.largeSpacing

    contentItem: ColumnLayout {
        spacing: Kirigami.Units.smallSpacing * 2

        Controls.Label {
            Layout.fillWidth: true
            text: model.entry.title
            font.pointSize: Math.round(Kirigami.Theme.defaultFont.pointSize * 1)
            font.weight: Font.Medium
            wrapMode: Text.Wrap
            elide: Text.ElideRight
            maximumLineCount: 3
            opacity: model.entry.read ? 0.7 : 1
        }

        Controls.Label {
            Layout.fillWidth: true
            elide: Text.ElideRight
            font.pointSize: Kirigami.Theme.smallFont.pointSize
            opacity: 0.9
            text: model.entry.updated.toLocaleString(Qt.locale(), Locale.ShortFormat) + (model.entry.authors.length === 0 ? "" : " " + i18nc("by <author(s)>", "by") + " " + model.entry.authors)
        }
    }
}

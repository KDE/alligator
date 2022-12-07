/**
 * SPDX-FileCopyrightText: 2020 Tobias Fella <fella@posteo.de>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

import QtQuick 2.14
import QtQuick.Controls 2.14 as Controls
import QtQuick.Layouts 1.14

import org.kde.kirigami 2.20 as Kirigami

import org.kde.alligator 1.0

Kirigami.Dialog {
    id: addSheet
    property string groupName: ""

    title: i18n("Add Feed")
    standardButtons: Kirigami.Dialog.Ok | Kirigami.Dialog.Cancel
    padding: Kirigami.Units.largeSpacing
    // bottomPadding: Kirigami.Units.largeSpacing + Kirigami.Units.smallSpacing
    preferredWidth: Kirigami.Units.gridUnit * 20

    onAccepted: {
        Database.addFeed(urlField.text, addSheet.groupName, markFeedAsRead.checked)
    }

    ColumnLayout {
        Controls.Label {
            text: i18n("Url:")
        }
        Controls.TextField {
            id: urlField
            Layout.fillWidth: true
            text: "https://planet.kde.org/global/atom.xml/"
        }
        Controls.CheckBox {
            id: markFeedAsRead
            checked: false
            text: i18n("Mark entries as read")
        }
    }
}

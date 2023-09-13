/**
 * SPDX-FileCopyrightText: 2020 Tobias Fella <tobias.fella@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

import QtQuick
import QtQuick.Controls as Controls
import QtQuick.Layouts

import org.kde.kirigami as Kirigami

import org.kde.alligator

Kirigami.Dialog {
    id: addSheet

    title: i18n("Add Feed")
    standardButtons: Kirigami.Dialog.Ok | Kirigami.Dialog.Cancel
    padding: Kirigami.Units.largeSpacing
    // bottomPadding: Kirigami.Units.largeSpacing + Kirigami.Units.smallSpacing
    preferredWidth: Kirigami.Units.gridUnit * 20

    onAccepted: {
        Database.addFeed(urlField.text, "", markFeedAsRead.checked)
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

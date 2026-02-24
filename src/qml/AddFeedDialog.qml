/**
 * SPDX-FileCopyrightText: 2020 Tobias Fella <tobias.fella@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

import QtQuick
import QtQuick.Controls as Controls
import QtQuick.Layouts

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard

import org.kde.alligator

Kirigami.PromptDialog {
    id: root

    title: i18n("Add Feed")
    preferredWidth: Kirigami.Units.gridUnit * 20

    standardButtons: Kirigami.Dialog.Ok | Kirigami.Dialog.Cancel

    Component.onCompleted: {
        standardButton(Kirigami.Dialog.Ok).enabled = Qt.binding(function() { return urlField.acceptableInput && urlField.text.trim() != "" });
    }


    onAccepted: {
        if (urlField.text.trim().length > 0) {
            Database.addFeed(urlField.text, "", markFeedAsRead.checked);
        }
    }

    onClosed: {
        urlField.text = "";
    }

    Kirigami.FormLayout {
        Controls.TextField {
            id: urlField
            implicitWidth: Kirigami.Units.gridUnit * 16
            Layout.fillWidth: true
            Kirigami.FormData.label: i18n("URL:")
            placeholderText: "https://planet.kde.org/global/atom.xml/"
            validator: RegularExpressionValidator { regularExpression: /\S+/ }
        }
        Controls.CheckBox {
            id: markFeedAsRead
            text: i18n("Mark entries as read")
            checked: false
        }
    }
}

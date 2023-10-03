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

FormCard.FormCardPage {
    id: root

    title: i18n("Add Feed")

    FormCard.FormCard {
        Layout.topMargin: Kirigami.Units.largeSpacing
        FormCard.FormTextFieldDelegate {
            id: urlField
            label: i18n("Url")
            placeholderText: "https://planet.kde.org/global/atom.xml/"
        }
        FormCard.FormDelegateSeparator {}
        FormCard.FormCheckDelegate {
            id: markFeedAsRead
            text: i18n("Mark entries as read")
            checked: false
        }
        FormCard.FormDelegateSeparator {}
        FormCard.FormButtonDelegate {
            text: i18nc("@action:button", "Add Feed")
            onClicked: {
                Database.addFeed(urlField.text, "", markFeedAsRead.checked)
                root.closeDialog()
            }
        }
        FormCard.FormButtonDelegate {
            text: i18nc("@action:button", "Cancel")
            onClicked: root.closeDialog()
        }
    }
}

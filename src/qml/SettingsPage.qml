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

Kirigami.ScrollablePage {
    title: i18n("Settings")

    Kirigami.FormLayout {

        Kirigami.Heading {
            Kirigami.FormData.isSection: true
            text: i18n("Article List")
        }

        RowLayout {
            Kirigami.FormData.label: i18n("Delete after:")

            Controls.SpinBox {
                id: deleteAfterCount
                value: Config.deleteAfterCount
                enabled: deleteAfterType.currentIndex !== 0

                onValueModified: Config.deleteAfterCount = value
            }

            Controls.ComboBox {
                id: deleteAfterType
                currentIndex: Config.deleteAfterType
                model: [i18n("Never"), i18n("Articles"), i18n("Days"), i18n("Weeks"), i18n("Months")]

                onActivated: Config.deleteAfterType = index
            }
        }

        Kirigami.Heading {
            Kirigami.FormData.isSection: true
            text: i18n("Article")
        }

        Controls.SpinBox {
            id: articleFontSizeSpinBox

            enabled: !useSystemFontCheckBox.checked
            value: Config.articleFontSize
            Kirigami.FormData.label: i18n("Font size:")
            from: 6
            to: 20

            onValueModified: Config.articleFontSize = value

        }

        Controls.CheckBox {
            id: useSystemFontCheckBox
            checked: Config.articleFontUseSystem
            text: i18n("Use system default")

            onToggled: Config.articleFontUseSystem = checked
        }

    }
}

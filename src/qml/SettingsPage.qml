/**
 * SPDX-FileCopyrightText: 2020 Tobias Fella <tobias.fella@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

import QtQuick
import QtQuick.Controls as Controls
import QtQuick.Layouts

import org.kde.kirigami as Kirigami

import org.kde.kirigamiaddons.formcard 1.0 as FormCard

import org.kde.alligator
import org.kde.alligator.config

FormCard.FormCardPage {
    id: root

    title: i18n("Settings")

    FormCard.FormHeader {
        title: i18nc("@title", "Article List")
    }
    FormCard.FormCard {
        FormCard.AbstractFormDelegate {
            contentItem: RowLayout {
                Controls.Label {
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignVCenter

                    text: i18n("Delete after:")
                    elide: Text.ElideRight
                    wrapMode: Text.Wrap
                    maximumLineCount: 2
                }
                Controls.SpinBox {
                    value: AlligatorSettings.deleteAfterCount
                    enabled: deleteAfterType.currentIndex !== 0
                    onValueChanged: AlligatorSettings.deleteAfterCount = value
                }
                Controls.ComboBox {
                    id: deleteAfterType
                    currentIndex: AlligatorSettings.deleteAfterType
                    model: [i18n("Never"), i18n("Articles"), i18n("Days"), i18n("Weeks"), i18n("Months")]

                    onActivated: index => AlligatorSettings.deleteAfterType = index
                }
            }
        }
    }

    FormCard.FormHeader {
        title: i18n("Article")
    }

    FormCard.FormCard {
        FormCard.FormSpinBoxDelegate {
            enabled: !useSystemFontCheckBox.checked
            value: AlligatorSettings.articleFontSize
            label: i18n("Font size:")
            from: 6
            to: 20

            onValueChanged: AlligatorSettings.articleFontSize = value
        }
        FormCard.FormCheckDelegate {
            id: useSystemFontCheckBox
            checked: AlligatorSettings.articleFontUseSystem
            text: i18n("Use system default")

            onToggled: AlligatorSettings.articleFontUseSystem = checked
        }
    }
}

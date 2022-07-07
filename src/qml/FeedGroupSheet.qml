/*
* SPDX-FileCopyrightText: 2021 Dimitris Kardarakos <dimkard@posteo.net>
*
* SPDX-License-Identifier: GPL-3.0-or-later
*/

import QtQuick 2.14
import QtQuick.Controls 2.14 as Controls
import QtQuick.Layouts 1.14
import org.kde.kirigami 2.12 as Kirigami
import org.kde.alligator 1.0 as Alligator

Kirigami.OverlaySheet {
    id: root

    property var groupsModel

    function clearFields() {
        name.text = "";
        desc.text = "";
    }

    header: Kirigami.Heading {
        text: i18n("Feed Group")
    }

    contentItem: Kirigami.FormLayout {
        Controls.TextField {
            id: name

            Layout.fillWidth: true
            Kirigami.FormData.label: i18n("Name:")
        }

        Controls.TextField {
            id: desc

            Layout.fillWidth: true
            Kirigami.FormData.label: i18n("Description:")
        }
    }

    footer: RowLayout {
        Item {
            Layout.fillWidth: true
        }

        Controls.ToolButton {
            text: i18n("OK")
            enabled: name.text !== ""

            onClicked: {
                Alligator.Database.addFeedGroup(name.text, desc.text);
                clearFields();
                root.close();
            }
        }

        Controls.ToolButton {
            text: i18n("Cancel")

            onClicked: {
                clearFields();
                root.close();
            }
        }
    }
}


/*
* SPDX-FileCopyrightText: 2021 Dimitris Kardarakos <dimkard@posteo.net>
*
* SPDX-License-Identifier: GPL-3.0-or-later
*/

import QtQuick
import QtQuick.Controls as Controls
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.alligator

Kirigami.Dialog {
    id: root

    property var groupsModel

    function clearFields() {
        name.text = "";
        description.text = "";
        setAsDefault.checked = false;
    }

    title: i18n("Feed Group")
    standardButtons: Kirigami.Dialog.Ok | Kirigami.Dialog.Cancel
    padding: Kirigami.Units.largeSpacing
    bottomPadding: Kirigami.Units.largeSpacing + Kirigami.Units.smallSpacing
    preferredWidth: Kirigami.Units.gridUnit * 20

    Component.onCompleted: {
        standardButton(Kirigami.Dialog.Ok).enabled = Qt.binding(function() { return !nameInUseWarning.visible && name.text.trim() != "" });
    }

    onAccepted: {
        Database.addFeedGroup(name.text.trim(), description.text.trim());

        if (setAsDefault.checked) {
            Database.setDefaultGroup(name.text.trim());
        }

        clearFields();
    }

    onRejected: clearFields();

    Kirigami.FormLayout {
        Controls.TextField {
            id: name
            Layout.fillWidth: true
            Kirigami.FormData.label: i18nc("@label:textbox", "Name:")
        }

        Kirigami.InlineMessage {
            id: nameInUseWarning
            text: i18n("A group with that name already exists.")
            type: Kirigami.MessageType.Warning
            visible: root.groupsModel.contains(name.text.trim())
            Layout.fillWidth: true
        }

        Controls.TextField {
            id: description
            Layout.fillWidth: true
            Kirigami.FormData.label: i18nc("@label:textbox", "Description:")
        }

        Controls.CheckBox {
            id: setAsDefault
            Layout.fillWidth: true
            Kirigami.FormData.label: i18nc("@option:check", "Set as default:")
        }
    }
}

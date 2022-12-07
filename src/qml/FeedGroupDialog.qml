/*
* SPDX-FileCopyrightText: 2021 Dimitris Kardarakos <dimkard@posteo.net>
*
* SPDX-License-Identifier: GPL-3.0-or-later
*/

import QtQuick 2.14
import QtQuick.Controls 2.14 as Controls
import QtQuick.Layouts 1.14
import org.kde.kirigami 2.20 as Kirigami
import org.kde.alligator 1.0 as Alligator

Kirigami.Dialog {
    id: root

    property var groupsModel

    function clearFields() {
        name.text = "";
        desc.text = "";
    }

    title: i18n("Feed Group")
    standardButtons: Kirigami.Dialog.Ok | Kirigami.Dialog.Cancel
    padding: Kirigami.Units.largeSpacing
    bottomPadding: Kirigami.Units.largeSpacing + Kirigami.Units.smallSpacing
    preferredWidth: Kirigami.Units.gridUnit * 20

    onAccepted: {
        Alligator.Database.addFeedGroup(name.text, desc.text);
        clearFields();
    }

    onRejected: {
        clearFields();
    }

    Kirigami.FormLayout {
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
}


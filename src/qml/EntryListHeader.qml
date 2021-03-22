/**
 * SPDX-FileCopyrightText: 2020 Tobias Fella <fella@posteo.de>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

import QtQuick 2.14
import QtQuick.Controls 2.14 as Controls
import QtQuick.Layouts 1.14

import org.kde.kirigami 2.12 as Kirigami

import org.kde.alligator 1.0

ColumnLayout {
    width: parent.width
    spacing: 0

    Kirigami.InlineMessage {
        type: Kirigami.MessageType.Error
        Layout.fillWidth: true
        Layout.margins: Kirigami.Units.smallSpacing
        text: i18n("Error (%1): %2", page.feed.errorId, page.feed.errorString)
        visible: page.feed.errorId !== 0
    }

    RowLayout {
        Layout.fillWidth: true
        Layout.margins: Kirigami.Units.smallSpacing
        spacing: Kirigami.Units.smallSpacing

        Kirigami.Icon {
            source: Fetcher.image(page.feed.image)
            width: height
            height: Kirigami.Units.gridUnit * 5
        }

        ColumnLayout {
            Kirigami.Heading {
                text: page.feed.name
            }
            Controls.Label {
                visible: text.length > 0
                text: page.feed.description
            }
            Controls.Label {
                visible: text.length > 0
                text: page.feed.authors.length === 0 ? "" : " " + i18nc("by <author(s)>", "by") + " " + page.feed.authors[0].name
            }
        }
    }
}

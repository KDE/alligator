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
    id: root

    property QtObject feed

    title: i18nc("<Feed Name> - Details", "%1 - Details", feed.displayName || feed.name)

    ColumnLayout {
        Kirigami.Icon {
            source: Fetcher.image(root.feed.image)
            height: 200
            width: height
        }
        Kirigami.Heading {
            text: root.feed.displayName || root.feed.name
        }
        Kirigami.Heading {
            text: root.feed.description
            level: 3
        }
        Controls.Label {
            text: i18nc("by <author(s)>", "by %1", root.feed.authors)
            visible: root.feed.authors.length !== 0
        }
        Controls.Label {
            text: "<a href='%1'>%1</a>".arg(root.feed.link)
            onLinkActivated: link => Qt.openUrlExternally(link)
        }
        Controls.Label {
            text: i18n("Subscribed since: %1", root.feed.subscribed.toLocaleString(Qt.locale(), Locale.ShortFormat))
        }
        Controls.Label {
            text: i18n("last updated: %1", root.feed.lastUpdated.toLocaleString(Qt.locale(), Locale.ShortFormat))
        }
        Controls.Label {
            text: i18n("%1 posts, %2 unread", root.feed.entryCount, feed.unreadEntryCount)
        }
    }
}

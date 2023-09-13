/**
 * SPDX-FileCopyrightText: 2020 Tobias Fella <tobias.fella@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

import QtQuick
import QtQuick.Controls as Controls
import QtQuick.Layouts

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.delegates as Delegates

import org.kde.alligator

Delegates.RoundedItemDelegate {
    id: delegate

    required property var feed

    signal editFeed(var feedObj)

    text: feed.displayName || feed.name
    icon: feed.refreshing ? "view-refresh" : feed.image === "" ? "rss" : Fetcher.image(delegate.feed.image)

    contentItem: RowLayout {
        Delegates.SubtitleContentItem {
            itemDelegate: delegate
            subtitle: i18np("%1 unread entry", "%1 unread entries", delegate.feed.unreadEntryCount)
        }

        Controls.ToolButton {
            icon.name: "delete"
            display: Controls.AbstractButton.IconOnly
            text: i18nc("'Feed' is an rss feed", "Delete this Feed")
            onClicked: {
                if(pageStack.depth > 1 && delegate.feed.url === lastFeed)
                    pageStack.pop()
                feedsModel.removeFeed(delegate.feed.url)
            }

            Controls.ToolTip.text: text
            Controls.ToolTip.visible: hovered
            Controls.ToolTip.delay: Kirigami.Units.toolTipDelay
        }

        Controls.ToolButton {
            icon.name: "editor"
            display: Controls.AbstractButton.IconOnly
            text: i18nc("'Feed' is an rss feed", "Edit this Feed")
            onClicked: editFeed(delegate.feed)

            Controls.ToolTip.text: text
            Controls.ToolTip.visible: hovered
            Controls.ToolTip.delay: Kirigami.Units.toolTipDelay
        }
    }
}

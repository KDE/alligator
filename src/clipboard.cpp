/**
 * SPDX-FileCopyrightText: 2025 Salvo 'LtWorf' Tomaselli <ltworf@debian.org>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */
#include "clipboard.h"

#include <QString>
#include <QClipboard>
#include <QGuiApplication>

void Clipboard::copy(const QString &text) {
    QGuiApplication::clipboard()->setText(text);
}

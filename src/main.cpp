/**
 * SPDX-FileCopyrightText: 2020 Tobias Fella <tobias.fella@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <QQuickView>
#include <QString>
#include <QStringList>

#ifdef Q_OS_ANDROID
#include <QGuiApplication>
#else
#include <QApplication>
#endif

#include <KAboutData>
#include <KLocalizedContext>
#include <KLocalizedString>

#include "alligator-version.h"
#include "alligatorsettings.h"
#include "contenthelper.h"
#include "database.h"
#include "entriesmodel.h"
#include "entriesproxymodel.h"
#include "feedgroupsmodel.h"
#include "feedsmodel.h"
#include "feedsproxymodel.h"
#include "fetcher.h"

#ifdef Q_OS_WINDOWS
#include <windows.h>
#endif

#ifdef Q_OS_ANDROID
Q_DECL_EXPORT
#endif

int main(int argc, char *argv[])
{
#ifdef Q_OS_ANDROID
    QGuiApplication app(argc, argv);
    QQuickStyle::setStyle(QStringLiteral("org.kde.breeze"));
#else
    QApplication app(argc, argv);
    if (qEnvironmentVariableIsEmpty("QT_QUICK_CONTROLS_STYLE")) {
        QQuickStyle::setStyle(QStringLiteral("org.kde.desktop"));
    }
#endif

#ifdef Q_OS_WINDOWS
    if (AttachConsole(ATTACH_PARENT_PROCESS)) {
        FILE *outFile = freopen("CONOUT$", "w", stdout);
        if (!outFile)
            qWarning() << "Failed to reopen stdout";
        FILE *errFile = freopen("CONOUT$", "w", stderr);
        if (!errFile)
            qWarning() << "Failed to reopen stderr";
    }

    QApplication::setStyle(QStringLiteral("breeze"));
    auto font = app.font();
    font.setPointSize(10);
    app.setFont(font);
#endif
    KLocalizedString::setApplicationDomain("alligator");

    QCoreApplication::setOrganizationName(QStringLiteral("KDE"));
    QCoreApplication::setOrganizationDomain(QStringLiteral("kde.org"));
    QCoreApplication::setApplicationName(QStringLiteral("Alligator"));

    KAboutData about(QStringLiteral("alligator"),
                     i18n("Alligator"),
                     QStringLiteral(ALLIGATOR_VERSION_STRING),
                     i18n("Feed Reader"),
                     KAboutLicense::GPL,
                     i18n("© 2020-2023 KDE Community"));
    about.addAuthor(i18n("Tobias Fella"), QString(), QStringLiteral("tobias.fella@kde.org"), QStringLiteral("https://tobiasfella.de"));
    KAboutData::setApplicationData(about);

    AlligatorSettings settings;
    qmlRegisterSingletonInstance("org.kde.alligator.config", 1, 0, "Config", &settings);

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextObject(new KLocalizedContext(&engine));

    QCommandLineParser parser;
    parser.setApplicationDescription(i18n("RSS/Atom Feed Reader"));
    QCommandLineOption addFeedOption(QStringList() << QStringLiteral("a") << QStringLiteral("addfeed"),
                                     i18n("Adds a new feed to database."),
                                     i18n("feed URL"),
                                     QStringLiteral("none"));
    parser.addOption(addFeedOption);

    about.setupCommandLine(&parser);
    parser.process(app);
    QString feedURL = parser.value(addFeedOption);
    if (feedURL != QStringLiteral("none"))
        Database::instance().addFeed(feedURL);
    about.processCommandLine(&parser);

    QObject::connect(&app, &QCoreApplication::aboutToQuit, &settings, &AlligatorSettings::save);

    Database::instance();

    engine.loadFromModule("org.kde.alligator", "Main");

    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    return app.exec();
}

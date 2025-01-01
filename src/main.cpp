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
#include <QTimer>

#ifdef Q_OS_ANDROID
#include <QGuiApplication>
#else
#include <QApplication>
#endif

#include <KAboutData>
#include <KColorSchemeManager>
#include <KLocalizedContext>
#include <KLocalizedString>

#include "alligator-version.h"
#include "alligatorsettings.h"
#include "database.h"

#ifdef Q_OS_WINDOWS
#include "debug.h"
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
    KColorSchemeManager colorMgr;

#ifdef Q_OS_WINDOWS
    if (AttachConsole(ATTACH_PARENT_PROCESS)) {
        FILE *outFile = freopen("CONOUT$", "w", stdout);
        if (!outFile)
            qCWarning(ALLIGATOR) << "Failed to reopen stdout";
        FILE *errFile = freopen("CONOUT$", "w", stderr);
        if (!errFile)
            qCWarning(ALLIGATOR) << "Failed to reopen stderr";
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
                     i18n("© 2020-2024 KDE Community"));
    about.addAuthor(i18n("Tobias Fella"), QString(), QStringLiteral("tobias.fella@kde.org"), QStringLiteral("https://tobiasfella.de"));
    KAboutData::setApplicationData(about);

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextObject(new KLocalizedContext(&engine));

    QCommandLineParser parser;
    parser.setApplicationDescription(i18n("RSS/Atom Feed Reader"));
    QCommandLineOption addFeedOption(QStringList() << QStringLiteral("a") << QStringLiteral("addfeed"),
                                     i18n("Adds a new feed to database."),
                                     i18n("feed URL"),
                                     QStringLiteral("none"));
    parser.addOption(addFeedOption);
    QCommandLineOption selfTestOpt(QStringLiteral("self-test"), QStringLiteral("internal, for automated testing"));
    parser.addOption(selfTestOpt);

    about.setupCommandLine(&parser);
    parser.process(app);
    QString feedURL = parser.value(addFeedOption);
    if (feedURL != QStringLiteral("none"))
        Database::instance().addFeed(feedURL);
    about.processCommandLine(&parser);

    QObject::connect(&app, &QCoreApplication::aboutToQuit, AlligatorSettings::self(), &AlligatorSettings::save);
    QObject::connect(&app, &QGuiApplication::applicationStateChanged, AlligatorSettings::self(), [](Qt::ApplicationState state) {
        if (state == Qt::ApplicationSuspended)
            AlligatorSettings::self()->save();
    });

    Database::instance();

    engine.loadFromModule("org.kde.alligator", "Main");

    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    if (parser.isSet(selfTestOpt)) {
        QTimer::singleShot(std::chrono::milliseconds(250), &app, &QCoreApplication::quit);
    }

    return app.exec();
}

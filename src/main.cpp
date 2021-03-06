/**
 * SPDX-FileCopyrightText: 2020 Tobias Fella <fella@posteo.de>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickView>
#include <QQuickStyle>
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
#include "database.h"
#include "entriesmodel.h"
#include "feedsmodel.h"
#include "fetcher.h"
#include "feedgroupsmodel.h"
#include "feedsproxymodel.h"

#ifdef Q_OS_ANDROID
Q_DECL_EXPORT
#endif

int main(int argc, char *argv[])
{
#ifdef Q_OS_ANDROID
    QGuiApplication app(argc, argv);
    QQuickStyle::setStyle(QStringLiteral("Material"));
#else
    QApplication app(argc, argv);
    if(qEnvironmentVariableIsEmpty("QT_QUICK_CONTROLS_STYLE")) {
        QQuickStyle::setStyle(QStringLiteral("org.kde.desktop"));
    }
#endif

#ifdef Q_OS_WINDOWS
    if (AttachConsole(ATTACH_PARENT_PROCESS)) {
        freopen("CONOUT$", "w", stdout);
        freopen("CONOUT$", "w", stderr);
    }

    QApplication::setStyle(QStringLiteral("breeze"));
    auto font = app.font();
    font.setPointSize(10);
    app.setFont(font);
#endif

    QCoreApplication::setOrganizationName(QStringLiteral("KDE"));
    QCoreApplication::setOrganizationDomain(QStringLiteral("kde.org"));
    QCoreApplication::setApplicationName(QStringLiteral("Alligator"));

    KAboutData about(QStringLiteral("alligator"), i18n("Alligator"), QStringLiteral(ALLIGATOR_VERSION_STRING), i18n("Feed Reader"), KAboutLicense::GPL, i18n("© 2020 KDE Community"));
    about.addAuthor(i18n("Tobias Fella"), QString(), QStringLiteral("fella@posteo.de"));
    KAboutData::setApplicationData(about);

    qmlRegisterType<FeedsModel>("org.kde.alligator", 1, 0, "FeedsModel");
    qmlRegisterType<FeedGroupsModel>("org.kde.alligator", 1, 0, "FeedGroupsModel");
    qmlRegisterType<FeedsProxyModel>("org.kde.alligator", 1, 0, "FeedsProxyModel");

    qmlRegisterUncreatableType<EntriesModel>("org.kde.alligator", 1, 0, "EntriesModel", QStringLiteral("Get from Feed"));

    qmlRegisterSingletonInstance("org.kde.alligator", 1, 0, "Fetcher", &Fetcher::instance());
    qmlRegisterSingletonInstance("org.kde.alligator", 1, 0, "Database", &Database::instance());

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextObject(new KLocalizedContext(&engine));
    KLocalizedString::setApplicationDomain("alligator");

    QCommandLineParser parser;
    parser.setApplicationDescription(i18n("RSS/Atom Feed Reader"));
    QCommandLineOption addFeedOption(QStringList() << QStringLiteral("a") << QStringLiteral("addfeed")
                                   , i18n("Adds a new feed to database.")
                                   , i18n("feed URL")
                                   , QStringLiteral("none"));
    parser.addOption(addFeedOption);

    about.setupCommandLine(&parser);
    parser.process(app);
    QString feedURL = parser.value(addFeedOption);
    if(feedURL != QStringLiteral("none"))
        Database::instance().addFeed(feedURL);
    about.processCommandLine(&parser);

    engine.rootContext()->setContextProperty(QStringLiteral("_aboutData"), QVariant::fromValue(about));

    AlligatorSettings settings;

    engine.rootContext()->setContextProperty(QStringLiteral("_settings"), &settings);

    QObject::connect(&app, &QCoreApplication::aboutToQuit, &settings, &AlligatorSettings::save);

    Database::instance();

    engine.load(QUrl(QStringLiteral("qrc:///main.qml")));

    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    return app.exec();
}

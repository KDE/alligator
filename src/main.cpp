/**
 * SPDX-FileCopyrightText: 2020 Tobias Fella <fella@posteo.de>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */


#include <QCommandLineParser>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickView>
#include <QQuickStyle>

#if defined(Q_OS_ANDROID) || defined(UBUNTU_TOUCH)
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

#include <QLibraryInfo>

#ifdef Q_OS_ANDROID
Q_DECL_EXPORT
#endif
int main(int argc, char *argv[])
{
#ifdef Q_OS_ANDROID
    QGuiApplication app(argc, argv);
    QQuickStyle::setStyle(QStringLiteral("Material"));
#elif defined(UBUNTU_TOUCH)
    QGuiApplication app(argc, argv);
    QIcon::setThemeSearchPaths(QIcon::themeSearchPaths() << QStringLiteral("/usr/share/icons") << QStringLiteral("share/icons"));
    QIcon::setThemeName(QStringLiteral("suru"));
    QIcon::setFallbackThemeName(QStringLiteral("breeze"));
#else
    QApplication app(argc, argv);
#endif

    qmlRegisterType<FeedsModel>("org.kde.alligator", 1, 0, "FeedsModel");
    qmlRegisterUncreatableType<EntriesModel>("org.kde.alligator", 1, 0, "EntriesModel", QStringLiteral("Get from Feed"));
    qmlRegisterSingletonType<Fetcher>("org.kde.alligator", 1, 0, "Fetcher", [](QQmlEngine *engine, QJSEngine *) -> QObject * {
        engine->setObjectOwnership(&Fetcher::instance(), QQmlEngine::CppOwnership);
        return &Fetcher::instance();
    });
    qmlRegisterSingletonType<Database>("org.kde.alligator", 1, 0, "Database", [](QQmlEngine *engine, QJSEngine *) -> QObject * {
        engine->setObjectOwnership(&Database::instance(), QQmlEngine::CppOwnership);
        return &Database::instance();
    });

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextObject(new KLocalizedContext(&engine));
    KLocalizedString::setApplicationDomain("alligator");

    QCommandLineParser parser;
    parser.setApplicationDescription(i18n("RSS/Atom Feed Reader"));

    KAboutData about(QStringLiteral("alligator"), i18n("Alligator"), QStringLiteral(ALLIGATOR_VERSION_STRING), i18n("Feed Reader"), KAboutLicense::GPL, i18n("© 2020 KDE Community"));
    about.addAuthor(i18n("Tobias Fella"), QString(), QStringLiteral("fella@posteo.de"));
    KAboutData::setApplicationData(about);

    about.setupCommandLine(&parser);
    parser.process(app);
    about.processCommandLine(&parser);

#ifdef UBUNTU_TOUCH
    QCoreApplication::setApplicationName(QStringLiteral("org.kde.alligator"));
    QCoreApplication::setOrganizationName(QString());
#endif

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

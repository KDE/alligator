# SPDX-FileCopyrightText: 2020 Tobias Fella <tobias.fella@kde.org>
# SPDX-License-Identifier: BSD-2-Clause

add_executable(alligator
    main.cpp
    feedsmodel.cpp
    entriesmodel.cpp
    entriesproxymodel.cpp
    fetcher.cpp
    database.cpp
    entry.cpp
    feed.cpp
    feedgroupsmodel.cpp
    feedsproxymodel.cpp
    resources.qrc
)

kconfig_add_kcfg_files(alligator alligatorsettings.kcfgc GENERATE_MOC)

target_include_directories(alligator PRIVATE ${CMAKE_BINARY_DIR})
target_link_libraries(alligator PRIVATE Qt::Core Qt::Qml Qt::Quick Qt::QuickControls2 Qt::Sql KF${QT_MAJOR_VERSION}::Syndication KF${QT_MAJOR_VERSION}::CoreAddons KF${QT_MAJOR_VERSION}::ConfigGui KF${QT_MAJOR_VERSION}::I18n)

if(ANDROID)
    target_link_libraries(alligator PRIVATE
        KF${QT_MAJOR_VERSION}::Kirigami2
        Qt::Svg
        OpenSSL::SSL
    )
    if(SQLite3_FOUND)
        target_link_libraries(alligator PRIVATE SQLite::SQLite3)
    endif()

    kirigami_package_breeze_icons(ICONS
        bookmark-remove
        bookmark-new
        delete
        settings-configure
        help-about-symbolic
        list-add
        view-refresh
        alligator
        mail-mark-read
        mail-read
        mail-mark-unread
        mail-sent
        globe
        data-error
        rss
        document-import
        document-export
        edit-group
        arrow-down
        arrow-up
    )
else()
    target_link_libraries(alligator PRIVATE Qt::Widgets)
endif()

install(TARGETS alligator ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

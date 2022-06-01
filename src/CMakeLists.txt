# SPDX-FileCopyrightText: 2020 Tobias Fella <fella@posteo.de>
# SPDX-License-Identifier: BSD-2-Clause

add_executable(alligator
    main.cpp
    about.cpp
    feedsmodel.cpp
    entriesmodel.cpp
    entriesproxymodel.cpp
    fetcher.cpp
    database.cpp
    entry.cpp
    feed.cpp
    author.cpp
    feedgroupsmodel.cpp
    feedsproxymodel.cpp
    resources.qrc
)

kconfig_add_kcfg_files(alligator alligatorsettings.kcfgc GENERATE_MOC)

target_include_directories(alligator PRIVATE ${CMAKE_BINARY_DIR})
target_link_libraries(alligator PRIVATE Qt5::Core Qt5::Qml Qt5::Quick Qt5::QuickControls2 Qt5::Sql KF5::Syndication KF5::CoreAddons KF5::ConfigGui KF5::I18n)

if(ANDROID)
    target_link_libraries(alligator PRIVATE
        KF5::Kirigami2
        Qt5::Svg
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
        mail-sent
        globe
        data-error
        rss
        document-import
        document-export
        edit-group
    )
else()
    target_link_libraries(alligator PRIVATE Qt5::Widgets)
endif()

install(TARGETS alligator ${KF5_INSTALL_TARGETS_DEFAULT_ARGS})

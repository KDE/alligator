# SPDX-FileCopyrightText: 2020 Tobias Fella <fella@posteo.de>
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
    author.cpp
    feedgroupsmodel.cpp
    feedsproxymodel.cpp
)

kconfig_add_kcfg_files(alligator alligatorsettings.kcfgc GENERATE_MOC)

target_include_directories(alligator PRIVATE ${CMAKE_BINARY_DIR})
target_link_libraries(alligator PRIVATE Qt::Core Qt::Qml Qt::Quick Qt::QuickControls2 Qt::Sql KF6::Syndication KF6::CoreAddons KF6::ConfigGui KF6::I18n)

qt_add_qml_module(alligator
    URI org.kde.alligator
    VERSION 1.0
    QML_FILES
    qml/AddFeedDialog.qml
    qml/AlligatorGlobalDrawer.qml
    qml/EditFeedDialog.qml
    qml/EntryListDelegate.qml
    qml/EntryListPage.qml
    qml/EntryPage.qml
    qml/FeedDetailsPage.qml
    qml/FeedGroupDialog.qml
    qml/FeedListDelegate.qml
    qml/FeedListPage.qml
    qml/GroupsListPage.qml
    qml/main.qml
    qml/SettingsPage.qml
)

if(ANDROID)
    target_link_libraries(alligator PRIVATE
        KF5::Kirigami2
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
    )
else()
    target_link_libraries(alligator PRIVATE Qt::Widgets)
endif()

install(TARGETS alligator ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

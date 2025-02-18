# SPDX-FileCopyrightText: 2020 Tobias Fella <tobias.fella@kde.org>
# SPDX-License-Identifier: BSD-2-Clause

add_executable(alligator
    main.cpp
    feedsmodel.cpp
    entriesmodel.cpp
    entriesproxymodel.cpp
    fetcher.cpp
    database.cpp
    feed.cpp
    feedgroupsmodel.cpp
    feedsproxymodel.cpp
    contenthelper.cpp
)

ecm_add_qml_module(alligator URI org.kde.alligator DEPENDENCIES QtCore)

ecm_target_qml_sources(alligator
    SOURCES
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
    qml/SettingsPage.qml
    qml/Main.qml
)

kconfig_add_kcfg_files(alligator alligatorsettings.kcfgc GENERATE_MOC)

ecm_qt_declare_logging_category(alligator
    HEADER debug.h
    IDENTIFIER ALLIGATOR
    CATEGORY_NAME org.kde.alligator
    DESCRIPTION "Alligator"
    EXPORT ALLIGATOR
)

target_include_directories(alligator PRIVATE ${CMAKE_BINARY_DIR})
target_link_libraries(alligator PRIVATE Qt::Core Qt::Qml Qt::Quick Qt::QuickControls2 Qt::Sql KF6::Syndication KF6::CoreAddons KF6::ConfigGui KF6::I18n KF6::ColorScheme)
if (NOT QT6_IS_SHARED_LIBS_BUILD)
    qt6_import_qml_plugins(alligator)
endif()

if(ANDROID)
    target_link_libraries(alligator PRIVATE
        KF6::Kirigami
        Qt::Svg
        OpenSSL::SSL
    )
    if(SQLite3_FOUND)
        target_link_libraries(alligator PRIVATE SQLite::SQLite3)
    endif()

    kirigami_package_breeze_icons(ICONS
        bookmarks
        bookmarks-bookmarked
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
        arrow-right
        license
        feed-subscribe
        documentinfo
        dialog-ok
        dialog-cancel
        editor
        favorite-symbolic
        sidebar-collapse
        sidebar-expand
    )
    ecm_add_android_apk(alligator ANDROID_DIR ${CMAKE_SOURCE_DIR}/android)
else()
    target_link_libraries(alligator PRIVATE Qt::Widgets)
endif()

install(TARGETS alligator ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

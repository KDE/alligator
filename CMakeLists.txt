# SPDX-FileCopyrightText: 2020 Tobias Fella <tobias.fella@kde.org>
# SPDX-FileCopyrightText: 2020 Nicolas Fella <nicolas.fella@gmx.de>
# SPDX-License-Identifier: BSD-2-Clause

cmake_minimum_required(VERSION 3.16)

# KDE Applications version, managed by release script.
set(RELEASE_SERVICE_VERSION_MAJOR "25")
set(RELEASE_SERVICE_VERSION_MINOR "11")
set(RELEASE_SERVICE_VERSION_MICRO "90")
set(RELEASE_SERVICE_VERSION "${RELEASE_SERVICE_VERSION_MAJOR}.${RELEASE_SERVICE_VERSION_MINOR}.${RELEASE_SERVICE_VERSION_MICRO}")

project(alligator VERSION ${RELEASE_SERVICE_VERSION})

set(KF_MIN_VERSION "6.12.0")
set(QT_MIN_VERSION "6.5.0")
set(CMAKE_CXX_STANDARD 20)

find_package(ECM ${KF_MIN_VERSION} REQUIRED NO_MODULE)

set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

set(KDE_COMPILERSETTINGS_LEVEL 6.0.0)

include(FeatureSummary)
include(ECMSetupVersion)
include(KDEInstallDirs)
include(KDEClangFormat)
include(KDECMakeSettings)
include(KDEGitCommitHooks)
include(ECMAddAndroidApk)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(ECMQmlModule)
include(ECMQtDeclareLoggingCategory)

ecm_setup_version(${PROJECT_VERSION}
    VARIABLE_PREFIX ALLIGATOR
    VERSION_HEADER ${CMAKE_CURRENT_BINARY_DIR}/alligator-version.h
)

find_package(Qt6 ${QT_MIN_VERSION} REQUIRED NO_MODULE COMPONENTS Core Quick Test Gui QuickControls2 Sql)
find_package(KF6 ${KF_MIN_VERSION} REQUIRED COMPONENTS CoreAddons Syndication Config I18n ColorScheme Kirigami)

find_package(KF6KirigamiAddons 0.6 REQUIRED)
if (NOT BUILD_SHARED_LIBS)
    find_package(QQC2BreezeStyle REQUIRED)
endif()

if (ANDROID)
    find_package(Qt6 ${QT_MIN_VERSION} REQUIRED COMPONENTS Svg)
    find_package(OpenSSL REQUIRED)
    find_package(SQLite3)
else()
    find_package(Qt6 ${QT_MIN_VERSION} REQUIRED COMPONENTS Widgets)
endif()

add_definitions(-DQT_NO_CAST_FROM_ASCII
                -DQT_NO_CAST_TO_ASCII
                -DQT_NO_URL_CAST_FROM_STRING
                -DQT_NO_NARROWING_CONVERSIONS_IN_CONNECT
                -DQT_USE_QSTRINGBUILDER
                -DQT_DISABLE_DEPRECATED_BEFORE=0x050d00
)

ki18n_install(po)

install(PROGRAMS org.kde.alligator.desktop DESTINATION ${KDE_INSTALL_APPDIR})
install(FILES org.kde.alligator.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})
install(FILES alligator.svg DESTINATION ${KDE_INSTALL_FULL_ICONDIR}/hicolor/scalable/apps)

add_subdirectory(src)
add_subdirectory(autotests)

ecm_qt_install_logging_categories(
    EXPORT ALLIGATOR
    FILE alligator.categories
    DESTINATION ${KDE_INSTALL_LOGGINGCATEGORIESDIR}
)


feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)

kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)

file(GLOB_RECURSE ALL_CLANG_FORMAT_SOURCE_FILES src/*.cpp src/*.h)
kde_clang_format(${ALL_CLANG_FORMAT_SOURCE_FILES})

cmake_minimum_required(VERSION 3.20)
project(godl)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(BUILD_WITH_QT6 ON)

find_package(ECM REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

# Directories where Qt Creator can find QML files.
# (Not needed for builds, but makes Qt Creator code completion happy.)
list(APPEND QML_IMPORT_PATH "/usr/lib/qt6/qml" "C:/CraftRoot/qml")

# Prevent adding duplicate values at each run of CMake.
list(REMOVE_DUPLICATES QML_IMPORT_PATH)

# The variable is cached in ${BUILD_DIR}/CMakeCache.txt. We need FORCE to
# change it there immediately. Also, add a comment to the cache file.
set(QML_IMPORT_PATH ${QML_IMPORT_PATH}
    CACHE STRING "Qt Creator 4.1 extra qml import paths"
    FORCE
)

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(ECMFindQmlModule)

find_package(Qt6 REQUIRED COMPONENTS
    Core
    Quick
    Test
    Gui
    QuickControls2
    Widgets
    Concurrent
)

find_package(KF6 REQUIRED COMPONENTS
    Kirigami
    KirigamiAddons
    I18n
    CoreAddons
    QQC2DesktopStyle
    IconThemes
    Config
    ConfigWidgets
    Archive
)

ecm_find_qmlmodule(org.kde.kirigami REQUIRED)
ecm_find_qmlmodule(org.kde.kirigamiaddons.components REQUIRED)
ecm_find_qmlmodule(org.kde.kirigamiaddons.formcard REQUIRED)
ecm_find_qmlmodule(org.kde.desktop REQUIRED)

message(NOTICE "Build type is " ${CMAKE_BUILD_TYPE})
if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    add_definitions(-DDEBUG=ON)
endif()

execute_process(
    COMMAND git rev-parse --short HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_COMMIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
    RESULT_VARIABLE GIT_COMMIT_HASH_RESULT
)

if(GIT_COMMIT_HASH_RESULT EQUAL 0)
    add_definitions(-DGIT_COMMIT_HASH="${GIT_COMMIT_HASH}")
else()
    add_definitions(-DGIT_COMMIT_HASH="unknown")
endif()

execute_process(
    COMMAND git describe --tags --abbrev=1
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_TAG
    OUTPUT_STRIP_TRAILING_WHITESPACE
    RESULT_VARIABLE GIT_TAG_RESULT
)

if(GIT_TAG_RESULT EQUAL 0)
    add_definitions(-DGIT_TAG="${GIT_TAG}")
else()
    add_definitions(-DGIT_TAG="unknown")
endif()

add_subdirectory(src)

install(PROGRAMS org.fishy.godl.desktop DESTINATION ${KDE_INSTALL_APPDIR})

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)

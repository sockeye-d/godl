cmake_minimum_required(VERSION 3.20)
project(godl)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(BUILD_WITH_QT6 ON)

option(ASAN "Enable AddressSanitizer" OFF)

if (${ASAN})
    message("AddressSanitizer enabled")
    add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address)
endif ()

find_package(ECM REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)

find_package(Qt6 REQUIRED COMPONENTS
        Core
        Test
        Widgets
        Concurrent
        Network
)

find_package(KF6 REQUIRED COMPONENTS
        BreezeIcons
        CoreAddons
        Config
        ConfigWidgets
        Archive
        WidgetsAddons
        XmlGui
        ColorScheme
        ItemViews
)

message(NOTICE "Build type is " ${CMAKE_BUILD_TYPE})
if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    add_definitions(-DDEBUG=ON)
endif ()

execute_process(
        COMMAND git rev-parse --short HEAD
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_COMMIT_HASH
        OUTPUT_STRIP_TRAILING_WHITESPACE
        RESULT_VARIABLE GIT_COMMIT_HASH_RESULT
)

if (GIT_COMMIT_HASH_RESULT EQUAL 0)
    add_definitions(-DGIT_COMMIT_HASH="${GIT_COMMIT_HASH}")
else ()
    add_definitions(-DGIT_COMMIT_HASH="unknown")
endif ()

execute_process(
        COMMAND git describe --tags --abbrev=1
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_TAG
        OUTPUT_STRIP_TRAILING_WHITESPACE
        RESULT_VARIABLE GIT_TAG_RESULT
)

if (GIT_TAG_RESULT EQUAL 0)
    add_definitions(-DGIT_TAG="${GIT_TAG}")
else ()
    add_definitions(-DGIT_TAG="unknown")
endif ()

add_subdirectory(src)

if (LINUX)
    include(GNUInstallDirs)
    install(FILES io.github.sockeye_d.godl.desktop DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/applications)
endif ()

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)

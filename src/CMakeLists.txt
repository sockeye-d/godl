qt_add_executable(godl
    main.cpp network/chainedjsonrequest.cpp network/chainedjsonrequest.h network/macros.h)

kconfig_add_kcfg_files(godl config.kcfgc GENERATE_MOC)

if (IS_READABLE "${CMAKE_SOURCE_DIR}/local.properties")
    # Read the token from local.properties
    file(READ "${CMAKE_SOURCE_DIR}/local.properties" LOCAL_PROPS_CONTENT)
    string(REGEX MATCH "GITHUB_TOKEN=([^\n\r]*)" _match "${LOCAL_PROPS_CONTENT}")
    set(GITHUB_TOKEN "${CMAKE_MATCH_1}")
    add_definitions(-DGITHUB_TOKEN="${CMAKE_MATCH_1}")
endif()


qt_add_qml_module(godl
    URI org.fishy.godl
    VERSION 0.1
    QML_FILES Main.qml
    QML_FILES RemoteVersionsPage.qml
    QML_FILES LocalVersionsPage.qml
    QML_FILES ProjectsPage.qml
    SOURCES util/util.h util/util.cpp
    SOURCES main.h
    SOURCES model/downloadmanagermodel.h model/downloadmanagermodel.cpp
    SOURCES network/downloadmanager.h network/downloadmanager.cpp
    SOURCES network/downloadinfo.h
    SOURCES network/chainedjsonrequest.h
    SOURCES network/network.h
    SOURCES network/networkresponsecode.h network/networkresponsecode.cpp
    RESOURCES config.kcfg
    RESOURCES config.kcfgc
    SOURCES util/circularbuffer.h util/circularbuffer.cpp
    SOURCES
    SOURCES model/downloadmanagermodel.cpp model/downloadmanagermodel.h
    SOURCES model/listbasedmodel.h model/listbasedmodel.cpp
)

ecm_find_qmlmodule(org.kde.kirigami)

target_sources(godl
    PRIVATE
    main.cpp
)

target_link_libraries(godl
    PRIVATE
    Qt6::Quick
    Qt6::Qml
    Qt6::Gui
    Qt6::QuickControls2
    Qt6::Widgets
    KF6::I18n
    KF6::CoreAddons
    KF6::IconThemes
    KF6::ConfigCore
    KF6::ConfigGui
)

target_include_directories(godl
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/network
)

install(TARGETS godl ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

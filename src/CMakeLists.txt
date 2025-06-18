qt_add_executable(godl
    main.cpp network/chainedjsonrequest.cpp network/chainedjsonrequest.h network/macros.h)

kconfig_add_kcfg_files(config_SRCS config.kcfgc GENERATE_MOC)

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
    QML_FILES ui/Main.qml
    QML_FILES ui/RemoteVersionsPage.qml
    QML_FILES ui/LocalVersionsPage.qml
    QML_FILES ui/ProjectsPage.qml
    QML_FILES
    QML_FILES
    QML_FILES ui/config/GeneralPage.qml
    SOURCES util/util.h util/util.cpp
    SOURCES main.h
    SOURCES model/downloadmanagermodel.h model/downloadmanagermodel.cpp
    SOURCES network/downloadmanager.h network/downloadmanager.cpp
    SOURCES network/downloadinfo.h
    SOURCES network/chainedjsonrequest.h
    SOURCES network/network.h
    SOURCES network/networkresponsecode.h network/networkresponsecode.cpp
    SOURCES util/circularbuffer.h util/circularbuffer.cpp
    SOURCES model/downloadmanagermodel.cpp model/downloadmanagermodel.h
    SOURCES model/listbasedmodel.h model/listbasedmodel.cpp
    SOURCES ${config_SRCS}
    RESOURCES config.kcfg
    RESOURCES config.kcfgc
    QML_FILES ui/config/DownloadsPage.qml
)

ecm_find_qmlmodule(org.kde.kirigami)
ecm_find_qmlmodule(org.kde.kirigamiaddons)

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
    KF6::ConfigWidgets
)

target_include_directories(godl
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/network
)

install(TARGETS godl ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

qt_add_executable(godl)

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
    NO_CACHEGEN
    SOURCES main.cpp main.h
    SOURCES util/util.h util/util.cpp
    SOURCES godlapp.h godlapp.cpp
    SOURCES configsignals.h configsignals.cpp
    SOURCES model/downloadmanagermodel.h model/downloadmanagermodel.cpp
    SOURCES network/downloadmanager.h network/downloadmanager.cpp
    SOURCES network/downloadinfo.h
    SOURCES network/chainedjsonrequest.h network/chainedjsonrequest.cpp
    SOURCES network/network.h network/network.h
    SOURCES network/networkresponsecode.h network/networkresponsecode.cpp
    SOURCES util/circularbuffer.h util/circularbuffer.cpp
    SOURCES model/downloadmanagermodel.h model/downloadmanagermodel.cpp
    SOURCES widgets/betterfiledialog.h widgets/betterfiledialog.cpp
    SOURCES registry/versionregistry.h registry/versionregistry.cpp
    SOURCES util/iconconverter.h util/iconconverter.cpp
    SOURCES registry/godotversion.h registry/godotversion.cpp
    SOURCES model/versionregistrymodel.h model/versionregistrymodel.cpp
    SOURCES ${config_SRCS}
    SOURCES registry/projectsregistry.h registry/projectsregistry.cpp
    SOURCES model/internalprojectsregistrymodel.h model/internalprojectsregistrymodel.cpp
    SOURCES registry/godotproject.h registry/godotproject.cpp
    SOURCES registry/boundgodotversion.h registry/boundgodotversion.cpp
    SOURCES serializable.h
    SOURCES model/abstractsimplelistmodel.h model/abstractsimplelistmodel.cpp
    SOURCES model/projectsregistrymodel.h model/projectsregistrymodel.cpp
    SOURCES util/dateconverter.h util/dateconverter.cpp
    SOURCES config.kcfg
    SOURCES config.kcfgc
    QML_FILES ui/Main.qml
    QML_FILES ui/RemoteVersionsPage.qml
    QML_FILES ui/LocalVersionsPage.qml
    QML_FILES ui/ProjectsPage.qml
    QML_FILES ui/config/GeneralConfigPage.qml
    QML_FILES ui/config/DownloadsConfigPage.qml
    QML_FILES ui/config/GodlConfigPage.qml
    QML_FILES ui/config/FormActionTextFieldDelegate.qml
    QML_FILES ui/PlatformActionMenuItem.qml
    QML_FILES ui/config/ConfigStringListFormCard.qml
    QML_FILES ui/config/ProjectsConfigPage.qml
    QML_FILES ui/DateLabel.qml
    QML_FILES ui/ProjectCard.qml
    QML_FILES ui/NewProjectPage.qml
    QML_FILES ui/LocalVersionCard.qml
)

target_link_libraries(godl
    PRIVATE
    Qt6::Core
    Qt6::Quick
    Qt6::Qml
    Qt6::Gui
    Qt6::QuickControls2
    Qt6::Widgets
    Qt6::Concurrent
    KF6::I18n
    KF6::CoreAddons
    KF6::IconThemes
    KF6::ConfigCore
    KF6::ConfigGui
    KF6::ConfigWidgets
    KF6::Archive
    KirigamiAddonsStatefulApp
)

target_include_directories(godl
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/model
)
target_include_directories(godl
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/network
)
target_include_directories(godl
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/util
)
target_include_directories(godl
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/widgets
)
target_include_directories(godl
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/registry
)

install(TARGETS godl ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

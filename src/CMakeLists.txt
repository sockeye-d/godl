find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS Core)

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
    SOURCES main.cpp main.h
    SOURCES util/util.h util/util.cpp
    SOURCES util/fileutil.h util/fileutil.cpp
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
    SOURCES projecttemplates.h projecttemplates.cpp
    SOURCES template.h template.cpp
    SOURCES cli/installcommand.h cli/installcommand.cpp
    SOURCES cli/parser.h cli/parser.cpp
    SOURCES cli/interface.h cli/interface.cpp
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
    RESOURCES resources.qrc
    SOURCES cli/ansi.h cli/ansi.cpp
    SOURCES cli/testcommand.h cli/testcommand.cpp
    SOURCES cli/versioncommand.h cli/versioncommand.cpp
    SOURCES cli/remotecommand.h cli/remotecommand.cpp
    SOURCES cli/editcommand.h cli/editcommand.cpp
    SOURCES cli/gconfigcommand.h cli/gconfigcommand.cpp
    SOURCES cli/importcommand.h cli/importcommand.cpp
    SOURCES cli/cliutil.h cli/cliutil.cpp
    SOURCES cli/createcommand.h cli/createcommand.cpp
    SOURCES templatedata.h templatedata.cpp
    SOURCES versioninfo.h versioninfo.cpp
    SOURCES util/singleton.h util/singleton.cpp
    SOURCES cli/run.h cli/run.cpp
    SOURCES util/sizeconverter.h util/sizeconverter.cpp
)

file(GLOB_RECURSE qml_modules CONFIGURE_DEPENDS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ui/*.qml)
qt_add_resources(godl "qml_modules" PREFIX "/" FILES ${qml_modules})
message(NOTICE "Adding the modules in ${qml_modules}")

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
    KF6::IconThemes
    KirigamiAddonsStatefulApp
)

if (WIN32)
    set(ADDITIONAL_DLLS
        "libbzip2.dll"
        "libcrypto-3-x64.dll"
        "liblzma-5.dll"
        "zlib1.dll"
        "libzstd.dll"
        "libintl-8.dll"
        "libKF6GuiAddons.dll"
        "libbrotlicommon.dll"
        "libbrotlidec.dll"
        "libb2-1.dll"
        "icuin76.dll"
        "icuuc76.dll"
        "icudt76.dll"
        "libpcre2-16.dll"
        "iconv.dll"
        "libfreetype-6.dll"
        "libharfbuzz-0.dll"
        "libpng16.dll"
        "libstdc++-6.dll"
        "libgomp-1.dll"
        "libgcc_s_seh-1.dll"
        "libwinpthread-1.dll"
        "libjpeg-62.dll"
        "libKF6ConfigQml.dll"
        "libKF6SonnetCore.dll"
        "libKirigami.dll"
        "libKirigamiDelegates.dll"
        "libKirigamiDialogs.dll"
        "libKirigamiLayouts.dll"
        "libKirigamiLayoutsPrivate.dll"
        "libKirigamiPlatform.dll"
        "libKirigamiPrimitives.dll"
        "libKirigamiPrivate.dll"
        "libsharpyuv.dll"
        "libtiff.dll"
        "libwebp.dll"
        "libwebpdemux.dll"
        "libwebpmux.dll"
    )

    add_custom_command(TARGET godl POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:godl> $<TARGET_FILE_DIR:godl> COMMAND_EXPAND_LISTS
    )

    foreach(DLL_NAME ${ADDITIONAL_DLLS})
        find_file(${DLL_NAME}_PATH ${DLL_NAME} PATHS ${CMAKE_PREFIX_PATH}/bin ENV PATH)
        if(${DLL_NAME}_PATH)
            add_custom_command(TARGET godl POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different ${${DLL_NAME}_PATH} $<TARGET_FILE_DIR:godl>
            )
        endif()
    endforeach()

    find_program(WINDEPLOYQT_EXE windeployqt REQUIRED)

    add_custom_command(TARGET godl POST_BUILD
        COMMAND ${WINDEPLOYQT_EXE}
        --qmldir ${CMAKE_CURRENT_BINARY_DIR}
        # --qmlimport ${CMAKE_CURRENT_BINARY_DIR}
        --no-translations
        $<TARGET_FILE:godl>
    )

    find_program(RCEDIT_EXE rcedit REQUIRED)

    add_custom_command(TARGET godl POST_BUILD
        COMMAND ${RCEDIT_EXE} $<TARGET_FILE:godl>
        --set-icon ${CMAKE_CURRENT_SOURCE_DIR}/godl.ico
    )
endif()

if (LINUX)
    set(DEPLOY_DIR ${CMAKE_BINARY_DIR}/deploy)
    add_custom_command(TARGET godl POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${DEPLOY_DIR}
    )

    add_custom_command(TARGET godl POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:godl> ${DEPLOY_DIR}/$<TARGET_FILE_NAME:godl>
    )

    # add_custom_command(TARGET godl POST_BUILD
    #     COMMAND ${CMAKE_COMMAND} -E copy_directory $<TARGET_FILE_DIR:godl>/org ${DEPLOY_DIR}/org
    # )
endif()

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

if (LINUX)
    include(GNUInstallDirs)
    install(PROGRAMS ${CMAKE_BINARY_DIR}/deploy/godl TYPE BIN)
    install(FILES icon-raster/16.png  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/16x16/apps    RENAME io.github.sockeye_d.godl.png)
    install(FILES icon-raster/32.png  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/32x32/apps    RENAME io.github.sockeye_d.godl.png)
    install(FILES icon-raster/48.png  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/48x48/apps    RENAME io.github.sockeye_d.godl.png)
    install(FILES icon-raster/64.png  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/64x64/apps    RENAME io.github.sockeye_d.godl.png)
    install(FILES icon-raster/96.png  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/96x96/apps    RENAME io.github.sockeye_d.godl.png)
    install(FILES icon-raster/128.png DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/128x128/apps  RENAME io.github.sockeye_d.godl.png)
    install(FILES icon-raster/256.png DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/256x256/apps  RENAME io.github.sockeye_d.godl.png)
    install(FILES icon-raster/512.png DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/512x512/apps  RENAME io.github.sockeye_d.godl.png)
    install(FILES icon-raster/512.png DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/512x512/apps  RENAME io.github.sockeye_d.godl.png)
    install(FILES godl.svg            DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/scalable/apps RENAME io.github.sockeye_d.godl.svg)
endif()

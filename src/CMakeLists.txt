kconfig_add_kcfg_files(config_SRCS config.kcfgc GENERATE_MOC)

if (IS_READABLE "${CMAKE_SOURCE_DIR}/local.properties")
    # Read the token from local.properties
    file(READ "${CMAKE_SOURCE_DIR}/local.properties" LOCAL_PROPS_CONTENT)
    string(REGEX MATCH "GITHUB_TOKEN=([^\n\r]*)" _match "${LOCAL_PROPS_CONTENT}")
    set(GITHUB_TOKEN "${CMAKE_MATCH_1}")
    add_definitions(-DGITHUB_TOKEN="${CMAKE_MATCH_1}")
endif ()

set(CMAKE_AUTORCC ON)

qt_add_executable(godl
        resources.qrc

        main.cpp main.h
        util/util.h util/util.cpp
        util/fileutil.h util/fileutil.cpp
        configsignals.h configsignals.cpp
        model/downloadmanagermodel.h model/downloadmanagermodel.cpp
        network/downloadmanager.h network/downloadmanager.cpp
        network/downloadinfo.h
        network/chainedjsonrequest.h network/chainedjsonrequest.cpp
        network/network.h network/network.h
        network/networkresponsecode.h network/networkresponsecode.cpp
        util/circularbuffer.h util/circularbuffer.cpp
        model/downloadmanagermodel.h model/downloadmanagermodel.cpp
        widgets/betterfiledialog.h widgets/betterfiledialog.cpp
        registry/versionregistry.h registry/versionregistry.cpp
        registry/godotversion.h registry/godotversion.cpp
        model/versionregistrymodel.h model/versionregistrymodel.cpp
        ${config_SRCS}
        registry/projectsregistry.h registry/projectsregistry.cpp
        model/internalprojectsregistrymodel.h model/internalprojectsregistrymodel.cpp
        registry/godotproject.h registry/godotproject.cpp
        registry/boundgodotversion.h registry/boundgodotversion.cpp
        serializable.h
        model/abstractsimplelistmodel.h model/abstractsimplelistmodel.cpp
        model/projectsregistrymodel.h model/projectsregistrymodel.cpp
        util/dateconverter.h util/dateconverter.cpp
        config.kcfg
        config.kcfgc
        projecttemplates.h projecttemplates.cpp
        template.h template.cpp
        cli/installcommand.h cli/installcommand.cpp
        cli/parser.h cli/parser.cpp
        cli/interface.h cli/interface.cpp
        cli/ansi.h cli/ansi.cpp
        cli/testcommand.h cli/testcommand.cpp
        cli/versioncommand.h cli/versioncommand.cpp
        cli/remotecommand.h cli/remotecommand.cpp
        cli/editcommand.h cli/editcommand.cpp
        cli/gconfigcommand.h cli/gconfigcommand.cpp
        cli/importcommand.h cli/importcommand.cpp
        cli/cliutil.h cli/cliutil.cpp
        cli/createcommand.h cli/createcommand.cpp
        templatedata.h templatedata.cpp
        versioninfo.h versioninfo.cpp
        util/singleton.h util/singleton.cpp
        cli/run.h cli/run.cpp
        util/sizeconverter.h util/sizeconverter.cpp
        ui/godlwindow.h
        ui/godlwindow.cpp
        ui/page.cpp
        ui/page.h
        ui/pages/ProjectsPage.cpp
        ui/pages/ProjectsPage.h
        ui/pages/LocalVersionsPage.h
        ui/pages/LocalVersionsPage.cpp
        ui/pages/RemoteVersionsPage.h
        ui/pages/RemoteVersionsPage.cpp
        ui/widgets/cardwidget.cpp
        ui/widgets/cardwidget.h
        ui/widgets/carddelegate.cpp
        ui/widgets/carddelegate.h
        util/units.cpp
        util/units.h
        ui/widgets/filelineedit.cpp
        ui/widgets/filelineedit.h
        ui/style.cpp
        ui/style.h
)

target_link_libraries(godl
        PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::Concurrent
        Qt6::Network
        KF6::CoreAddons
        KF6::BreezeIcons
        KF6::ConfigCore
        KF6::ConfigGui
        KF6::ConfigWidgets
        KF6::Archive
        KF6::WidgetsAddons
        KF6::XmlGui
        KF6::ColorScheme
        KF6::ItemViews
)

if (WIN32)
    set(ADDITIONAL_DLLS
            "libbzip2.dll"
            "libbz2.dll"
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
            "libKirigamiPolyfill.dll"
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

    foreach (DLL_NAME ${ADDITIONAL_DLLS})
        find_file(${DLL_NAME}_PATH ${DLL_NAME} PATHS ${CMAKE_PREFIX_PATH}/bin ENV PATH)
        if (${DLL_NAME}_PATH)
            add_custom_command(TARGET godl POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${${DLL_NAME}_PATH} $<TARGET_FILE_DIR:godl>
            )
        endif ()
    endforeach ()

    find_program(WINDEPLOYQT_EXE windeployqt REQUIRED)

    add_custom_command(TARGET godl POST_BUILD
            COMMAND ${WINDEPLOYQT_EXE}
            --no-translations
            $<TARGET_FILE:godl>
    )

    find_program(RCEDIT_EXE rcedit REQUIRED)

    add_custom_command(TARGET godl POST_BUILD
            COMMAND ${RCEDIT_EXE} $<TARGET_FILE:godl>
            --set-icon ${CMAKE_CURRENT_SOURCE_DIR}/godl.ico
    )
endif ()

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
endif ()

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
    install(FILES icon-raster/16.png DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/16x16/apps RENAME io.github.sockeye_d.godl.png)
    install(FILES icon-raster/32.png DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/32x32/apps RENAME io.github.sockeye_d.godl.png)
    install(FILES icon-raster/48.png DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/48x48/apps RENAME io.github.sockeye_d.godl.png)
    install(FILES icon-raster/64.png DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/64x64/apps RENAME io.github.sockeye_d.godl.png)
    install(FILES icon-raster/96.png DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/96x96/apps RENAME io.github.sockeye_d.godl.png)
    install(FILES icon-raster/128.png DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/128x128/apps RENAME io.github.sockeye_d.godl.png)
    install(FILES icon-raster/256.png DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/256x256/apps RENAME io.github.sockeye_d.godl.png)
    install(FILES icon-raster/512.png DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/512x512/apps RENAME io.github.sockeye_d.godl.png)
    install(FILES icon-raster/512.png DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/512x512/apps RENAME io.github.sockeye_d.godl.png)
    install(FILES godl.svg DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/scalable/apps RENAME io.github.sockeye_d.godl.svg)
endif ()

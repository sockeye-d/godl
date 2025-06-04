add_executable(godl
    network/chainedjsonrequest.cpp network/chainedjsonrequest.h network/macros.h)

if (IS_READABLE "${CMAKE_SOURCE_DIR}/local.properties")
    # Read the token from local.properties
    file(READ "${CMAKE_SOURCE_DIR}/local.properties" LOCAL_PROPS_CONTENT)
    string(REGEX MATCH "GITHUB_TOKEN=([^\n\r]*)" _match "${LOCAL_PROPS_CONTENT}")
    set(GITHUB_TOKEN "${CMAKE_MATCH_1}")
    add_definitions(-DGITHUB_TOKEN="${CMAKE_MATCH_1}")
endif()

# ecm_add_qml_module(godl
#     URI
#     org.fishy.godl
# )

qt_add_qml_module(godl
    URI org.fishy.godl
    VERSION 0.1
    SOURCES
    network/chainedjsonrequest.h
    QML_FILES
        Main.qml
        SOURCES util/util.h
        SOURCES main.h
        SOURCES network/godotdownloadmanager.h network/godotdownloadmanager.cpp
)

target_sources(godl
    PRIVATE
    main.cpp
)

# ecm_target_qml_sources(godl
#     SOURCES
#     Main.qml
# )

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
)

target_include_directories(godl
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/network
)

install(TARGETS godl ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

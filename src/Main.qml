// Includes relevant modules used by the QML
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls
import org.kde.kirigami as Kirigami

import org.fishy.godl

// Provides basic features needed for all kirigami applications
Kirigami.ApplicationWindow {
    // Unique identifier to reference this object
    id: root

    property string text

    width: Kirigami.Units.gridUnit * 45
    height: Kirigami.Units.gridUnit * 30

    // Window title
    // i18nc() makes a string translatable
    // and provides additional context for the translators
    title: i18nc("@title:window", "godl")

    globalDrawer: Kirigami.GlobalDrawer {
        isMenu: true
        actions: [
            Kirigami.Action {
                text: i18n("About")
                icon.name: "help-about"
                onTriggered: aboutPage.show()
            }
        ]
    }

    Kirigami.AbstractApplicationWindow {
        id: aboutPage
        transientParent: parent
        modality: Qt.ApplicationModal
        visible: false
        width: Kirigami.Units.gridUnit * 25
        height: Kirigami.Units.gridUnit * 20
        Kirigami.AboutPage {
            anchors.fill: parent
            aboutData: About
        }
    }

    DownloadManager {
        id: dl
    }

    pageStack.initialPage: Kirigami.Page {
        id: mainPage
        property int activePageIndex: 0
        // anchors.fill: parent
        Controls.ActionGroup {
            id: actionGroup
        }

        actions: [
            Kirigami.Action {
                checkable: true
                icon.name: "edit"
                text: "Projects"
                checked: mainPage.activePageIndex === 0
                onTriggered: mainPage.activePageIndex = 0
                Controls.ActionGroup.group: actionGroup
            },
            Kirigami.Action {
                checkable: true
                icon.name: "drive"
                text: "Local versions"
                checked: mainPage.activePageIndex === 1
                onTriggered: mainPage.activePageIndex = 1
                Controls.ActionGroup.group: actionGroup
            },
            Kirigami.Action {
                checkable: true
                icon.name: "server-symbolic"
                text: "Remote versions"
                checked: mainPage.activePageIndex === 2
                onTriggered: mainPage.activePageIndex = 2
                Controls.ActionGroup.group: actionGroup
            }
        ]

        ColumnLayout {
            anchors.fill: parent

            Controls.SwipeView {
                id: swipeView
                Layout.fillWidth: true
                Layout.fillHeight: true

                currentIndex: mainPage.activePageIndex
                onCurrentIndexChanged: {
                    mainPage.activePageIndex = currentIndex
                }

                ProjectsPage {
                    title: "Projects"
                }

                LocalVersionsPage {
                    title: "Local versions"
                }

                RemoteVersionsPage {
                    id: dlPage
                    title: "Remote versions"
                    Component.onCompleted: {
                        dlPage.dl = dl
                    }
                }
            }
        }
    }
}

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

    minimumWidth: Kirigami.Units.gridUnit * 10
    minimumHeight: Kirigami.Units.gridUnit * 10

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
                enabled: pageStack.layers.currentItem !== aboutPage
                onTriggered: pageStack.layers.push(aboutPage)
            }
        ]
    }

    Kirigami.AboutPage {
        id: aboutPage
        aboutData: About
    }

    DownloadManager {
        id: dl
        onDownloadStarted: notificationPopup.open()
    }

    footer: RowLayout {
        Layout.fillWidth: true
        Item {
            Layout.fillWidth: true
        }

        Controls.Button {
            id: notificationPopupToggle
            checkable: true
            checked: notificationPopup.visible
            onCheckedChanged: if (checked) {
                                  notificationPopup.open()
                              } else {
                                  notificationPopup.close()
                              }

            flat: true
            icon.name: "download"

            Controls.Popup {
                id: notificationPopup
                x: parent.width - width
                y: -height
                width: Kirigami.Units.gridUnit * 10.0
                height: parent.checked
                        && visible ? Kirigami.Units.gridUnit * 20.0 : 0.0

                Behavior on height {
                    NumberAnimation {
                        duration: Kirigami.Units.longDuration
                        easing.type: Easing.BezierSpline
                        easing.bezierCurve: [0.25, 0.0, 0.25, 1.0, 1.0, 1.0]
                    }
                }

                ListView {
                    anchors.fill: parent

                    model: dl.model

                    delegate: RowLayout {
                        required property var assetName
                        required property var progress
                        Layout.fillWidth: true
                        Controls.Label {
                            text: assetName === null ? "null name??" : assetName
                        }

                        Controls.ProgressBar {
                            width: Kirigami.Units.gridUnit * 2.0
                            value: progress
                            indeterminate: progress < 0.0
                        }
                    }
                }
            }
        }
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

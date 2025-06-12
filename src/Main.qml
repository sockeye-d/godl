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
                width: Kirigami.Units.gridUnit * 20.0
                height: notificationCardsScroll.height
                z: 10000

                Behavior on height {
                    NumberAnimation {
                        duration: Kirigami.Units.longDuration
                        easing.type: Easing.OutExpo
                    }
                }

                Controls.ScrollView {
                    id: notificationCardsScroll
                    width: parent.width
                    height: Math.min(notificationCards.height,
                                     Kirigami.Units.gridUnit * 15.0)
                    Controls.ScrollBar.horizontal.policy: Controls.ScrollBar.AlwaysOff
                    ColumnLayout {
                        id: notificationCards
                        width: parent.width
                        spacing: Kirigami.Units.largeSpacing * 2

                        Repeater {
                            width: notificationCards.width
                            model: dl.model

                            delegate: Kirigami.AbstractCard {
                                required property var assetName
                                required property var progress
                                required property var id
                                required property var downloadSpeed

                                width: notificationCards.width

                                headerOrientation: Qt.Horizontal

                                header: Kirigami.Heading {
                                    level: 2
                                    text: assetName
                                }

                                contentItem: RowLayout {
                                    width: notificationCards.width
                                    Controls.Label {
                                        text: `${downloadSpeed.toFixed(
                                                  2)} MiB/s`
                                    }

                                    Kirigami.Separator {
                                        Layout.fillHeight: true
                                    }

                                    Controls.ProgressBar {
                                        Layout.fillWidth: true
                                        width: Kirigami.Units.gridUnit * 2.0
                                        value: progress
                                        indeterminate: progress < 0.0
                                    }

                                    Controls.Button {
                                        text: i18n("Cancel")
                                        icon.name: "stop"
                                        onClicked: dl.cancel(id)
                                    }
                                }
                            }
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

        StackLayout {
            id: swipeView
            anchors.fill: parent

            currentIndex: mainPage.activePageIndex
            onCurrentIndexChanged: mainPage.activePageIndex = currentIndex

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

                StackLayout.onIsCurrentItemChanged: if (!hasContent
                                                            && StackLayout.isCurrentItem) {
                                                        refresh()
                                                    }
            }
        }
    }
}

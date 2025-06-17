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

    height: Kirigami.Units.gridUnit * 30
    minimumHeight: Kirigami.Units.gridUnit * 10
    minimumWidth: Kirigami.Units.gridUnit * 10

    // Window title
    // i18nc() makes a string translatable
    // and provides additional context for the translators
    title: i18nc("@title:window", "godl")
    width: Kirigami.Units.gridUnit * 45

    footer: RowLayout {
        Layout.fillWidth: true

        Item {
            Layout.fillWidth: true
        }
        Controls.Button {
            id: notificationPopupToggle

            checkable: true
            checked: notificationPopup.visible
            flat: true
            icon.name: "download"

            onCheckedChanged: if (checked) {
                notificationPopup.open();
            } else {
                notificationPopup.close();
            }

            Controls.Popup {
                id: notificationPopup

                property real wantedHeight

                closePolicy: Controls.Popup.NoAutoClose
                // this is so cursed... but it works
                height: wantedHeight == -1 ? (visible ? notificationCardsScroll.height + padding * 2.0 : 0.0) : wantedHeight
                rightPadding: 0
                width: Math.min(Kirigami.Units.gridUnit * 20.0, Math.round(root.width / 2))
                x: parent.width - width
                y: -height
                z: 10000

                Behavior on height {
                    NumberAnimation {
                        duration: Kirigami.Units.longDuration
                        easing.type: Easing.OutExpo
                    }
                }

                onAboutToHide: wantedHeight = 0
                onAboutToShow: wantedHeight = -1

                Controls.ScrollView {
                    id: notificationCardsScroll

                    Controls.ScrollBar.horizontal.policy: Controls.ScrollBar.AlwaysOff
                    clip: true
                    height: Math.min(notificationCards.height + notificationPopup.padding * 2.0, Math.min(Kirigami.Units.gridUnit * 15.0, Math.round(root.height / 2)))
                    width: notificationPopup.availableWidth

                    ColumnLayout {
                        id: notificationCards

                        spacing: Kirigami.Units.largeSpacing * 2
                        width: notificationCardsScroll.availableWidth - Kirigami.Units.largeSpacing

                        Repeater {
                            id: repeater

                            Layout.fillWidth: true
                            model: dl.model

                            delegate: Kirigami.AbstractCard {
                                required property var assetName
                                required property var downloadSpeed
                                required property var id
                                required property var progress

                                Layout.fillWidth: true
                                clip: true
                                headerOrientation: Qt.Horizontal

                                contentItem: RowLayout {
                                    width: notificationCards.width

                                    Controls.Label {
                                        elide: Text.ElideRight
                                        text: `${downloadSpeed.toFixed(2)} MiB/s`
                                    }
                                    Kirigami.Separator {
                                        Layout.fillHeight: true
                                    }
                                    Controls.ProgressBar {
                                        Layout.fillWidth: true
                                        indeterminate: progress < 0.0
                                        value: progress
                                        width: Kirigami.Units.gridUnit * 2.0
                                    }
                                    Controls.Button {
                                        icon.name: "stop"
                                        text: i18n("Cancel")

                                        onClicked: dl.cancel(id)
                                    }
                                }
                                header: Kirigami.Heading {
                                    elide: Text.ElideRight
                                    level: 2
                                    text: assetName
                                }
                            }
                        }
                    }
                }
                Controls.Label {
                    anchors.centerIn: parent
                    text: i18n("No active downloads")
                    visible: repeater.count === 0
                }
            }
        }
    }
    globalDrawer: Kirigami.GlobalDrawer {
        isMenu: true

        actions: [
            Kirigami.Action {
                enabled: pageStack.layers.currentItem !== aboutPage
                icon.name: "help-about"
                text: i18n("About")

                onTriggered: pageStack.layers.push(aboutPage)
            },
            Kirigami.Action {
                icon.name: "settings"
                text: i18n("Settings")

                onTriggered: ConfigDialog.open()
            }
        ]
    }
    pageStack.initialPage: Kirigami.Page {
        id: mainPage

        property int activePageIndex: 0
        property list<Kirigami.Action> baseActions: [
            Kirigami.Action {
                Controls.ActionGroup.group: actionGroup
                checkable: true
                checked: mainPage.activePageIndex === 0
                icon.name: "edit"
                text: "Projects"

                onTriggered: mainPage.activePageIndex = 0
            },
            Kirigami.Action {
                Controls.ActionGroup.group: actionGroup
                checkable: true
                checked: mainPage.activePageIndex === 1
                icon.name: "drive"
                text: "Local versions"

                onTriggered: mainPage.activePageIndex = 1
            },
            Kirigami.Action {
                Controls.ActionGroup.group: actionGroup
                checkable: true
                checked: mainPage.activePageIndex === 2
                icon.name: "server-symbolic"
                text: "Remote versions"

                onTriggered: mainPage.activePageIndex = 2
            }
        ]

        actions: swipeView.children[swipeView.currentIndex].actions.concat(baseActions)

        // anchors.fill: parent
        Controls.ActionGroup {
            id: actionGroup

        }
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
                    dlPage.dl = dl;
                }
                StackLayout.onIsCurrentItemChanged: if (!hasContent && StackLayout.isCurrentItem) {
                    refresh();
                }
            }
        }
    }

    Kirigami.AboutPage {
        id: aboutPage

        aboutData: About
    }
    DownloadManager {
        id: dl

        onDownloadStarted: notificationPopup.open()
    }
}

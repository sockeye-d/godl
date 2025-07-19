import QtQuick
import QtQuick.Layouts
import Qt.labs.platform as Platform
import QtQuick.Controls
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard
import org.kde.kirigamiaddons.settings as KirigamiSettings
import org.kde.kirigamiaddons.statefulapp as StatefulApp
import org.kde.desktop as KdeDesktop

import org.fishy.godl

import "config" as Configuration

StatefulApp.StatefulWindow {
    id: root

    property string text

    height: Kirigami.Units.gridUnit * 30
    minimumHeight: Kirigami.Units.gridUnit * 10
    minimumWidth: Kirigami.Units.gridUnit * 10
    width: Kirigami.Units.gridUnit * 45
    windowName: i18nc("@title:window", "godl")

    application: GodlApp {
        id: app

        configurationView: KirigamiSettings.ConfigurationView {
            modules: [
                KirigamiSettings.ConfigurationModule {
                    category: "general"
                    // @disable-check M17
                    icon.name: "configure"
                    moduleId: "general"
                    page: () => generalConfigPage
                    text: i18n("General")
                },
                KirigamiSettings.ConfigurationModule {
                    category: "downloads"
                    // @disable-check M17
                    icon.name: "download"
                    moduleId: "downloads"
                    page: () => downloadsConfigPage
                    text: i18n("Downloads")
                },
                KirigamiSettings.ConfigurationModule {
                    category: "project"
                    // @disable-check M17
                    icon.name: "project-development"
                    moduleId: "project"
                    page: () => projectsConfigPage
                    text: i18n("Projects")
                }
            ]

            // hey so this is cursed
            onConfigViewItemChanged: {
                if (!configViewItem)
                    return;
                configViewItem.flags = Qt.Dialog;
                configViewItem.transientParent = root;
                configViewItem.modality = Qt.ApplicationModal;
                // hey so this is more cursed
                // never do this
                configViewItem.visible = false;
                configViewItem.visible = true;
            }
        }
    }
    footer: RowLayout {
        Layout.fillWidth: true

        Item {
            Layout.fillWidth: true
        }

        Button {
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

            Popup {
                id: notificationPopup

                property real wantedHeight

                closePolicy: Popup.NoAutoClose
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

                ScrollView {
                    id: notificationCardsScroll

                    ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
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

                            delegate: Kirigami.Card {
                                id: card

                                required property string assetName
                                required property real downloadSpeed
                                required property string error
                                required property var id
                                required property real progress
                                required property int stage

                                Layout.fillWidth: true

                                actions: [
                                    Kirigami.Action {
                                        enabled: card.stage !== DownloadInfo.Unzipping
                                        icon.name: "dialog-cancel"

                                        onTriggered: dl.cancel(card.id)
                                    }
                                ]
                                contentItem: ColumnLayout {
                                    width: notificationCards.width

                                    Kirigami.InlineMessage {
                                        Layout.fillWidth: true
                                        text: card.error
                                        type: Kirigami.MessageType.Error
                                        visible: card.error != ""
                                    }

                                    Kirigami.InlineMessage {
                                        Layout.fillWidth: true
                                        text: i18n("Installation complete")
                                        type: Kirigami.MessageType.Positive
                                        visible: card.stage === DownloadInfo.Finished
                                    }

                                    RowLayout {
                                        Layout.fillWidth: true
                                        visible: card.stage !== DownloadInfo.Finished && card.error === ""

                                        Label {
                                            Layout.preferredWidth: Kirigami.Units.gridUnit * 4
                                            elide: Text.ElideRight
                                            text: card.stage === DownloadInfo.Downloading ? `${card.downloadSpeed.toFixed(2)} MiB/s` : i18n("Extracting")
                                        }

                                        Kirigami.Separator {
                                            Layout.fillHeight: true
                                        }

                                        ProgressBar {
                                            Layout.fillWidth: true
                                            indeterminate: card.progress < 0.0
                                            value: card.progress
                                            width: Kirigami.Units.gridUnit * 2.0
                                        }
                                    }
                                }
                                header: Kirigami.Heading {
                                    elide: Text.ElideRight
                                    level: 2
                                    text: card.assetName
                                }
                            }
                        }
                    }
                }

                Label {
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
                fromQAction: root.application.action("open_about_page")
            },
            Kirigami.Action {
                fromQAction: root.application.action("options_configure_keybinding")
            },
            Kirigami.Action {
                fromQAction: root.application.action("options_configure")
            }
        ]
    }
    pageStack.initialPage: Kirigami.Page {
        id: mainPage

        property int activePageIndex: 0
        property list<Kirigami.Action> baseActions: [
            Kirigami.Action {
                separator: true
            },
            Kirigami.Action {
                ActionGroup.group: actionGroup
                checkable: true
                checked: mainPage.activePageIndex === 0
                displayHint: Kirigami.DisplayHint.KeepVisible
                icon.name: "document-edit"
                text: "Projects"

                onTriggered: mainPage.activePageIndex = 0
            },
            Kirigami.Action {
                ActionGroup.group: actionGroup
                checkable: true
                checked: mainPage.activePageIndex === 1
                displayHint: Kirigami.DisplayHint.KeepVisible
                icon.name: "drive"
                text: "Local versions"

                onTriggered: mainPage.activePageIndex = 1
            },
            Kirigami.Action {
                ActionGroup.group: actionGroup
                checkable: true
                checked: mainPage.activePageIndex === 2
                displayHint: Kirigami.DisplayHint.KeepVisible
                icon.name: "globe"
                text: "Remote versions"

                onTriggered: mainPage.activePageIndex = 2
            }
        ]

        actions: swipeView.children[swipeView.currentIndex].actions.concat(baseActions)

        ActionGroup {
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

    Component.onCompleted: {
        deferredComponent.createObject(root);
    }

    Kirigami.ApplicationWindow {
        id: aboutPage

        height: Kirigami.Units.gridUnit * 25
        modality: Qt.WindowModal
        visible: false
        width: Kirigami.Units.gridUnit * 40

        pageStack.initialPage: FormCard.AboutPage {
        }
    }

    Component {
        id: generalConfigPage

        Configuration.GeneralConfigPage {
        }
    }

    Component {
        id: downloadsConfigPage

        Configuration.DownloadsConfigPage {
        }
    }

    Component {
        id: projectsConfigPage

        Configuration.ProjectsConfigPage {
        }
    }

    DownloadManager {
        id: dl

        onDownloadStarted: notificationPopup.open()
    }

    Component {
        id: deferredComponent

        Item {
            Action {
                shortcut: root.application.action("godl-next-page").shortcut

                onTriggered: if (mainPage.activePageIndex !== 2)
                    mainPage.activePageIndex++
            }

            Action {
                shortcut: root.application.action("godl-prev-page").shortcut

                onTriggered: if (mainPage.activePageIndex !== 0)
                    mainPage.activePageIndex--
            }
        }
    }
}

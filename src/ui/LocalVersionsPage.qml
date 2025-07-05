import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard

import org.fishy.godl

Kirigami.Page {
    padding: 0
    title: i18n("Local versions")

    actions: [
        Kirigami.Action {
            icon.name: "edit-clear-all"
            text: i18n("Clean leaked binaries")
            tooltip: i18n("Delete from the filesystem the binaries downloaded but not listed here")

            onTriggered: leakedVersionDialog.open()
        }
    ]

    Kirigami.Dialog {
        id: leakedVersionDialog

        title: i18n("Cleaning leaked binaries")

        property list<var> leaks

        standardButtons: leaks.length > 0 ? Controls.Dialog.Ok | Controls.Dialog.Cancel : Controls.Dialog.Ok

        width: Kirigami.Units.gridUnit * 30

        padding: Kirigami.Units.largeSpacing
        contentItem: ColumnLayout {
            Repeater {
                id: repeater
                delegate: Controls.CheckBox {
                    id: leakLayout

                    required property string path
                    required property string confirm
                    required property int index
                    Layout.fillWidth: true
                    text: leakLayout.path
                    checked: leakLayout.confirm
                    onCheckedChanged: leakedVersionDialog.leaks[index].confirm = checked
                }
            }

            Kirigami.PlaceholderMessage {
                Layout.fillWidth: true
                Layout.fillHeight: true
                // @disable-check M17
                icon.name: "edit-clear-all"
                text: i18n("No leaked versions")
                visible: repeater.count === 0
            }
        }

        onAboutToShow: {
            let l = VersionRegistry.detectLeakedVersions();
            leaks = [];
            for (let leak of l) {
                leaks.push({
                    path: leak,
                    confirm: true
                });
            }

            repeater.model = [];
            repeater.model = leaks;
        }

        onAccepted: if (leaks.length > 0)
            VersionRegistry.deleteLeakedVersions(leaks.filter(x => x.confirm).map(x => x.path))
    }

    Controls.ScrollView {
        Controls.ScrollBar.horizontal.policy: Controls.ScrollBar.AlwaysOff
        Layout.fillHeight: true
        Layout.fillWidth: true
        anchors.fill: parent

        Kirigami.CardsListView {
            id: versionsView
            Layout.fillWidth: true
            clip: true
            model: VersionRegistry.model

            delegate: Kirigami.Card {
                id: card

                required property GodotVersion modelData

                banner.title: modelData.assetName

                actions: [
                    Kirigami.Action {
                        icon.name: "folder-open"
                        text: i18n("Show folder")

                        onTriggered: card.modelData.showExternally()
                    },
                    Kirigami.Action {
                        icon.name: "system-run-symbolic"
                        text: i18n("Run")

                        onTriggered: card.modelData.start()
                    },
                    Kirigami.Action {
                        icon.name: "dialog-scripts"
                        text: i18n("Edit command")

                        onTriggered: commandDialog.open()
                    },
                    Kirigami.Action {
                        icon.name: "delete"
                        text: i18n("Remove")

                        onTriggered: VersionRegistry.removeVersion(card.modelData)
                    }
                ]
                contentItem: Controls.Label {
                    text: modelData.tag
                }

                Kirigami.Dialog {
                    id: commandDialog

                    height: Kirigami.Units.gridUnit * 15
                    padding: Kirigami.Units.largeSpacing
                    standardButtons: Kirigami.Dialog.Ok | Kirigami.Dialog.Cancel
                    title: i18n("Edit command")
                    width: Kirigami.Units.gridUnit * 40

                    customFooterActions: [
                        Kirigami.Action {
                            enabled: textField.text !== Config.defaultCommand
                            icon.name: "reset"
                            text: i18n("Reset")

                            onTriggered: textField.text = Config.defaultCommand
                        }
                    ]
                    footerLeadingComponent: ColumnLayout {
                        Layout.fillHeight: true
                        Layout.fillWidth: true

                        Repeater {
                            Layout.fillWidth: true
                            model: [
                                {
                                    "replacement": "{executable}",
                                    "description": "Expands to the absolute executable path"
                                },
                                {
                                    "replacement": "{projectPath}",
                                    "description": "Expands to the absolute project.godot path"
                                }
                            ]

                            delegate: RowLayout {
                                id: expansionsDelegate

                                required property string description
                                required property string replacement

                                Layout.fillWidth: true

                                Controls.Label {
                                    Layout.fillWidth: true
                                    elide: Text.ElideRight
                                    text: expansionsDelegate.replacement
                                }

                                Controls.Label {
                                    Layout.fillWidth: true
                                    elide: Text.ElideRight
                                    text: expansionsDelegate.description
                                }
                            }
                        }
                    }

                    onAccepted: card.modelData.cmd = textField.text

                    Controls.TextArea {
                        id: textField

                        Layout.fillHeight: true
                        Layout.fillWidth: true
                        text: card.modelData.cmd
                    }
                }
            }
        }
    }

    Kirigami.PlaceholderMessage {
        anchors.centerIn: parent
        // @disable-check M17
        icon.name: "edit-none"
        text: i18n("No versions have been downloaded")
        visible: versionsView.count === 0
    }
}

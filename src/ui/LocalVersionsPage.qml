import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls
import org.kde.kirigami as Kirigami

import org.fishy.godl

Kirigami.Page {
    title: i18n("Local versions")

    Kirigami.CardsListView {
        anchors.fill: parent
        model: VersionRegistry.model

        delegate: Kirigami.Card {
            id: card

            required property GodotVersion modelData

            banner.title: modelData.assetName

            actions: [
                Kirigami.Action {
                    icon.name: "folder-open"
                    text: i18n("Show folder")
                },
                Kirigami.Action {
                    icon.name: "debug-run"
                    text: i18n("Run")
                },
                Kirigami.Action {
                    icon.name: "debug-run"
                    text: i18n("Edit command")

                    onTriggered: commandDialog.open()
                },
                Kirigami.Action {
                    icon.name: "delete"
                    text: i18n("Remove")
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
                                replacement: "{executable}",
                                description: "Expands to the absolute executable path"
                            },
                            {
                                replacement: "{projectPath}",
                                description: "Expands to the absolute project.godot path"
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

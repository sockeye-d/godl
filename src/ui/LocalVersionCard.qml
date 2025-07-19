import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard

import org.fishy.godl

Kirigami.Card {
    id: root

    required property GodotVersion modelData

    banner.title: modelData.toString()

    actions: [
        Kirigami.Action {
            icon.name: "folder-open"
            text: i18n("Show folder")

            onTriggered: root.modelData.showExternally()
        },
        Kirigami.Action {
            icon.name: "system-run-symbolic"
            text: i18n("Run")

            onTriggered: root.modelData.start()
        },
        Kirigami.Action {
            icon.name: "dialog-scripts"
            text: i18n("Edit command")

            onTriggered: commandDialog.open()
        },
        Kirigami.Action {
            icon.name: "open-link"
            text: i18n("Show online")
            visible: root.modelData.repo[0] === "/"

            onTriggered: Qt.openUrlExternally(`https://github.com${root.modelData.repo}/releases/${root.modelData.tag}`)
        },
        Kirigami.Action {
            icon.name: "delete"
            text: i18n("Remove")

            onTriggered: VersionRegistry.removeVersion(root.modelData)
        }
    ]
    contentItem: Item {
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
                enabled: textField.text !== Configuration.defaultCommand
                icon.name: "reset"
                text: i18n("Reset")

                onTriggered: textField.text = Configuration.defaultCommand
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

        onAccepted: root.modelData.cmd = textField.text

        Controls.TextArea {
            id: textField

            Layout.fillHeight: true
            Layout.fillWidth: true
            text: root.modelData.cmd
        }
    }
}

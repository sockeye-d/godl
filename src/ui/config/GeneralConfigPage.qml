import QtQuick
import QtQuick.Layouts
import QtQuick.Dialogs as Dialogs
import QtQuick.Controls as Controls
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard
import org.kde.kirigamiaddons.settings as KirigamiSettings

import org.fishy.godl
import org.fishy.godl.qwidgets as QWidgets

GodlConfigPage {
    title: i18n("General")

    FormCard.FormCard {
        FormActionTextFieldDelegate {
            id: godotLocationField

            label: i18n("Godot location")
            text: Config.godotLocation

            rightActions: [
                Kirigami.Action {
                    icon.name: "folder-open"

                    onTriggered: {
                        folderDialog.startDirectory = Config.godotLocation;
                        folderDialog.open();
                    }
                },
                Kirigami.Action {
                    enabled: Config.godotLocation !== Config.defaultGodotLocationValue
                    icon.name: "document-revert"

                    onTriggered: Config.godotLocation = Config.defaultGodotLocationValue
                }
            ]

            onTextChanged: Config.godotLocation = text

            Connections {
                function onGodotLocationChanged() {
                    godotLocationField.text = Config.godotLocation;
                }

                target: Config
            }
        }

        FormActionTextFieldDelegate {
            id: projectLocationField

            label: i18n("Projects location")
            text: Config.projectLocation

            rightActions: [
                Kirigami.Action {
                    icon.name: "folder-open"

                    onTriggered: {
                        folderDialog.startDirectory = Config.projectLocation;
                        folderDialog.open();
                    }
                },
                Kirigami.Action {
                    enabled: Config.projectLocation !== Config.defaultProjectLocationValue
                    icon.name: "document-revert"

                    onTriggered: Config.projectLocation = Config.defaultProjectLocationValue
                }
            ]

            onTextChanged: Config.projectLocation = text

            Connections {
                function onProjectLocationChanged() {
                    projectLocationField.text = Config.projectLocation;
                }

                target: Config
            }
        }
    }

    FormCard.FormHeader {
        title: i18n("Developer options")
    }

    FormCard.FormCard {
        FormCard.FormCheckDelegate {
            checked: Config.cacheVersions
            text: i18n("Cache downloaded versions")

            onCheckedChanged: Config.cacheVersions = checked
        }
    }

    BetterFileDialog {
        id: folderDialog

        fileFilters: BetterFileDialog.Hidden | BetterFileDialog.NoDotAndDotDot
        mode: QWidgets.FileDialog.Directory

        onAccepted: path => Config.godotLocation = path
    }
}

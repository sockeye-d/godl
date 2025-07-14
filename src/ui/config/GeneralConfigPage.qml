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
            text: Configuration.godotLocation

            rightActions: [
                Kirigami.Action {
                    icon.name: "folder-open"

                    onTriggered: {
                        folderDialog.startDirectory = Configuration.godotLocation;
                        folderDialog.open();
                    }
                },
                Kirigami.Action {
                    enabled: Configuration.godotLocation !== Configuration.defaultGodotLocationValue
                    icon.name: "document-revert"

                    onTriggered: Configuration.godotLocation = Configuration.defaultGodotLocationValue
                }
            ]

            onTextChanged: Configuration.godotLocation = text

            Connections {
                function onGodotLocationChanged() {
                    godotLocationField.text = Configuration.godotLocation;
                }

                target: Configuration
            }
        }

        FormActionTextFieldDelegate {
            id: projectLocationField

            label: i18n("Projects location")
            text: Configuration.projectLocation

            rightActions: [
                Kirigami.Action {
                    icon.name: "folder-open"

                    onTriggered: {
                        folderDialog.startDirectory = Configuration.projectLocation;
                        folderDialog.open();
                    }
                },
                Kirigami.Action {
                    enabled: Configuration.projectLocation !== Configuration.defaultProjectLocationValue
                    icon.name: "document-revert"

                    onTriggered: Configuration.projectLocation = Configuration.defaultProjectLocationValue
                }
            ]

            onTextChanged: Configuration.projectLocation = text

            Connections {
                function onProjectLocationChanged() {
                    projectLocationField.text = Configuration.projectLocation;
                }

                target: Configuration
            }
        }
    }

    FormCard.FormHeader {
        title: i18n("Developer options")
    }

    FormCard.FormCard {
        FormCard.FormCheckDelegate {
            checked: Configuration.cacheVersions
            text: i18n("Cache downloaded versions")

            onCheckedChanged: Configuration.cacheVersions = checked
        }
    }

    BetterFileDialog {
        id: folderDialog

        fileFilters: BetterFileDialog.Hidden | BetterFileDialog.NoDotAndDotDot
        mode: QWidgets.FileDialog.Directory

        onAccepted: path => Configuration.godotLocation = path
    }
}

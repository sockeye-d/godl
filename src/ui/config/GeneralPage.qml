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
        anchors.left: parent.left
        anchors.right: parent.right

        FormActionTextFieldDelegate {
            id: godotLocationField

            label: "Godot location"
            text: Config.godotLocation

            rightActions: Kirigami.Action {
                icon.name: "folder-open"

                onTriggered: {
                    folderDialog.startDirectory = Config.godotLocation;
                    folderDialog.open();
                }
            }

            onTextChanged: Config.godotLocation = text

            Connections {
                function onGodotLocationChanged() {
                    godotLocationField.text = Config.godotLocation;
                }

                target: Config
            }
        }
    }
    BetterFileDialog {
        id: folderDialog

        fileFilters: BetterFileDialog.Hidden | BetterFileDialog.NoDotAndDotDot
        mode: QWidgets.FileDialog.Directory

        onAccepted: path => Config.godotLocation = path
    }
}

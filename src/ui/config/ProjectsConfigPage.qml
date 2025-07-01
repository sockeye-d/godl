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
            id: projectLocationField

            label: i18n("Projects location")
            text: Config.projectLocation

            rightActions: [
                Kirigami.Action {
                    icon.name: "folder-open"

                    onTriggered: folderDialog.open()
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

    BetterFileDialog {
        id: folderDialog

        fileFilters: BetterFileDialog.Hidden | BetterFileDialog.NoDotAndDotDot
        mode: QWidgets.FileDialog.Directory
        startDirectory: Config.projectLocation

        onAccepted: path => Config.projectLocation = path
    }
}

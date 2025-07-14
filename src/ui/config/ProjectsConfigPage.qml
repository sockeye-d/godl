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
            id: projectLocationField

            label: i18n("Projects location")
            text: Configuration.projectLocation

            rightActions: [
                Kirigami.Action {
                    icon.name: "folder-open"

                    onTriggered: folderDialog.open()
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

    BetterFileDialog {
        id: folderDialog

        fileFilters: BetterFileDialog.Hidden | BetterFileDialog.NoDotAndDotDot
        mode: QWidgets.FileDialog.Directory
        startDirectory: Configuration.projectLocation

        onAccepted: path => Configuration.projectLocation = path
    }
}

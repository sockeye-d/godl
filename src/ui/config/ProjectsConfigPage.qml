import QtQuick
import QtQuick.Layouts
import QtQuick.Dialogs as Dialogs
import QtQuick.Controls as Controls
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard
import org.kde.kirigamiaddons.settings as KirigamiSettings

import dev.fishies.godl
import dev.fishies.godl.qwidgets as QWidgets

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

        RowLayout {
            FormCard.FormComboBoxDelegate {
                id: defaultTemplateField

                Layout.fillWidth: true
                model: ProjectTemplates.templates
                text: i18n("Default project template")

                Component.onCompleted: {
                    ProjectTemplates.rescan();
                    currentIndex = ProjectTemplates.templates.indexOf(Configuration.defaultTemplate);
                }
                onCurrentTextChanged: Configuration.defaultTemplate = currentText
            }

            FormCard.FormButtonDelegate {
                Layout.fillHeight: true
                Layout.fillWidth: false
                icon.name: "view-refresh"
                trailingLogo.visible: false

                onClicked: {
                    ProjectTemplates.rescan();
                    defaultTemplateField.currentIndex = ProjectTemplates.templates.indexOf(Configuration.defaultTemplate);
                }
            }
        }
    }

    BetterFileDialog {
        id: folderDialog

        fileFilters: BetterFileDialog.Hidden | BetterFileDialog.NoDotAndDotDot
        mode: QWidgets.QFileDialog.Directory
        startDirectory: Configuration.projectLocation

        onAccepted: path => Configuration.projectLocation = path
    }
}

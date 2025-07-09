import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.components as KirigamiAddons
import org.kde.kirigamiaddons.formcard as FormCard

import org.fishy.godl
import org.fishy.godl.qwidgets as QWidgets

import "."

Kirigami.Page {
    id: root

    padding: 0

    actions: [
        Kirigami.Action {
            icon.name: "search"

            displayComponent: Kirigami.SearchField {
                id: textFilter

                text: ProjectsRegistry.model.filter

                onTextChanged: ProjectsRegistry.model.filter = textFilter.text
            }
        },
        Kirigami.Action {
            id: caseInsensitiveToggle

            checkable: true
            checked: ProjectsRegistry.model.filterCaseInsensitive
            icon.name: "format-text-superscript"

            onCheckedChanged: ProjectsRegistry.model.filterCaseInsensitive = caseInsensitiveToggle.checked
        },
        Kirigami.Action {
            displayComponent: Controls.ComboBox {
                id: sortByComboBox

                currentIndex: ProjectsRegistry.model.sortBy
                model: [i18n("Name"), i18n("Modified Date")]

                onCurrentIndexChanged: ProjectsRegistry.model.sortBy = currentIndex
            }
        },
        Kirigami.Action {
            icon.name: ProjectsRegistry.model.ascending ? "view-sort-ascending" : "view-sort-descending"

            onTriggered: ProjectsRegistry.model.ascending = !ProjectsRegistry.model.ascending
        },
        Kirigami.Action {
            icon.name: "document-properties"
            text: i18n("Manage")

            Kirigami.Action {
                icon.name: "view-list-tree"
                text: i18n("Scan")

                onTriggered: scanDialog.open()
            }

            Kirigami.Action {
                icon.name: "document-import"
                text: i18n("Import")

                onTriggered: importDialog.open()
            }

            Kirigami.Action {
                icon.color: Kirigami.Theme.positiveTextColor
                icon.name: "project-development-new-template"
                text: i18n("New project")

                onTriggered: applicationWindow().pageStack.layers.push(Qt.resolvedUrl("NewProjectPage.qml"))
            }
        }
    ]

    ColumnLayout {
        anchors.fill: parent

        Repeater {
            id: loadErrorRepeater

            model: ProjectsRegistry.loadErrors

            delegate: Kirigami.InlineMessage {
                id: message

                required property int index
                required property string modelData

                Layout.fillWidth: true
                Layout.preferredHeight: implicitHeight
                // @disable-check M17
                icon.color: Kirigami.Theme.negativeTextColor
                // @disable-check M17
                icon.name: "drive"
                showCloseButton: true
                text: `${i18n("Failed to load")} ${modelData}`
                type: Kirigami.MessageType.Error
                visible: false

                Timer {
                    interval: message.index * 20
                    running: true

                    onTriggered: message.visible = true
                }
            }
        }

        Controls.Button {
            Layout.fillWidth: true
            // flat: true
            icon.name: "edit-clear-all"
            text: i18n("Clear all")
            visible: loadErrorRepeater.visible && loadErrorRepeater.count >= 1

            onClicked: loadErrorRepeater.model = []
        }

        Controls.ScrollView {
            Controls.ScrollBar.horizontal.policy: Controls.ScrollBar.AlwaysOff
            Layout.fillHeight: true
            Layout.fillWidth: true

            GridView {
                id: projectsView

                Layout.fillWidth: true
                cellWidth: Math.floor(width / Math.max(1, Math.round(root.width / (Kirigami.Units.gridUnit * 40))))
                clip: true
                model: ProjectsRegistry.model
                reuseItems: false

                delegate: ProjectCard {
                    // height: Kirigami.Units.gridUnit * 10
                    // height: projectsView.cellHeight - Kirigami.Units.largeSpacing
                    width: projectsView.cellWidth - Kirigami.Units.largeSpacing

                    onHeightChanged: projectsView.cellHeight = Math.floor(height + Kirigami.Units.largeSpacing)
                    onTagSelected: tag => ProjectsRegistry.model.filter = `tag:${tag}`
                }
            }
        }
    }

    Kirigami.PlaceholderMessage {
        anchors.centerIn: parent
        explanation: i18n("Try scanning a directory or import a single project to get started")
        // @disable-check M17
        icon.name: "edit-none"
        text: i18n("No projects have been imported")
        visible: projectsView.count === 0

        KirigamiAddons.SegmentedButton {
            Layout.alignment: Qt.AlignHCenter

            actions: [
                Kirigami.Action {
                    icon.name: "view-list-tree"
                    text: i18n("Scan a whole directory tree")

                    onTriggered: scanDialog.open()
                },
                Kirigami.Action {
                    icon.name: "document-import"
                    text: i18n("Import a single project")

                    onTriggered: importDialog.open()
                }
            ]
        }
    }

    BetterFileDialog {
        id: scanDialog

        fileFilters: BetterFileDialog.Hidden | BetterFileDialog.NoDotAndDotDot
        mode: QWidgets.FileDialog.Directory
        startDirectory: Config.projectLocation

        onAccepted: path => ProjectsRegistry.scan(path)
    }

    BetterFileDialog {
        id: importDialog

        fileFilters: BetterFileDialog.Hidden | BetterFileDialog.NoDotAndDotDot
        mode: QWidgets.FileDialog.ExistingFile
        startDirectory: Config.projectLocation

        onAccepted: path => ProjectsRegistry.import(path)
    }
}

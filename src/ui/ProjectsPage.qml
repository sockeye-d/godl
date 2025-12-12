import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.components as KirigamiAddons
import org.kde.kirigamiaddons.formcard as FormCard

import dev.fishies.godl
import dev.fishies.godl.qwidgets as QWidgets

import "."

Kirigami.Page {
    id: root

    signal findVersion(string source, string tag)

    padding: 0

    actions: [
        Kirigami.Action {
            icon.name: "search"

            displayComponent: Kirigami.SearchField {
                id: textFilter

                text: ProjectsRegistry.model.filter

                onTextChanged: ProjectsRegistry.model.filter = textFilter.text

                Connections {
                    function onTriggered() {
                        ProjectsRegistry.model.filter = "";
                    }

                    target: textFilter.rightActions[0]
                }
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
                icon.name: "document-new-from-template"
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

            Kirigami.InlineMessage {
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
            icon.name: "edit-clear-all"
            text: i18n("Clear all")
            visible: [...Array(loadErrorRepeater.count).keys()].some(x => loadErrorRepeater.itemAt(x).visible)

            onClicked: [...Array(loadErrorRepeater.count).keys()].map(x => loadErrorRepeater.itemAt(x).visible = false)
        }

        Controls.ScrollView {
            Controls.ScrollBar.horizontal.policy: Controls.ScrollBar.AlwaysOff
            Layout.fillHeight: true
            Layout.fillWidth: true
            visible: !ProjectsRegistry.scanning

            GridView {
                id: projectsView

                property int recomputeHeight: 0

                Layout.fillWidth: true
                cacheBuffer: 2147483647
                cellHeight: {
                    recomputeHeight;
                    return Math.max(...[...Array(projectsView.count).keys()].map(x => projectsView.itemAtIndex(x)?.height)) + Kirigami.Units.largeSpacing;
                }
                cellWidth: Math.floor(width / Math.max(1, Math.round(root.width / (Kirigami.Units.gridUnit * 40))))
                clip: true
                model: ProjectsRegistry.model

                // reuseItems: true

                delegate: ProjectCard {
                    width: projectsView.cellWidth - Kirigami.Units.largeSpacing

                    onFindVersion: (a, b) => root.findVersion(a, b)
                    onTagSelected: tag => ProjectsRegistry.model.filter = `tag:${tag}`
                }

                onCountChanged: recomputeHeightTimer.restart()

                Connections {
                    function onFilterChanged() {
                        // ????
                        projectsView.width += 1;
                        projectsView.width -= 1;
                    }

                    target: ProjectsRegistry.model
                }

                Timer {
                    id: recomputeHeightTimer

                    interval: 0

                    onTriggered: projectsView.recomputeHeight++
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
        visible: projectsView.count === 0 && ProjectsRegistry.model.filter === "" && !ProjectsRegistry.scanning

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

    Kirigami.PlaceholderMessage {
        anchors.centerIn: parent
        // @disable-check M17
        icon.name: "edit-none"
        text: i18n("No projects matched your query")
        visible: projectsView.count === 0 && ProjectsRegistry.model.filter !== ""
    }

    Kirigami.LoadingPlaceholder {
        anchors.centerIn: parent
        text: i18n("Scanning...")
        visible: ProjectsRegistry.scanning
    }

    BetterFileDialog {
        id: scanDialog

        fileFilters: BetterFileDialog.NoDotAndDotDot
        mode: QWidgets.QFileDialog.Directory
        startDirectory: Configuration.projectLocation

        onAccepted: path => ProjectsRegistry.scan(path)
    }

    BetterFileDialog {
        id: importDialog

        fileFilters: BetterFileDialog.NoDotAndDotDot
        filters: ["Godot project files (*.godot)", "All files (*)"]
        mode: QWidgets.QFileDialog.ExistingFile
        startDirectory: Configuration.projectLocation

        onAccepted: path => ProjectsRegistry.import(path)
    }
}

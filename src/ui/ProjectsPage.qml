import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.components as KirigamiAddons
import org.kde.kirigamiaddons.formcard as FormCard

import org.fishy.godl
import org.fishy.godl.qwidgets as QWidgets

Kirigami.Page {
    id: root

    padding: 0

    actions: [
        Kirigami.Action {
            icon.name: "search"

            displayComponent: Kirigami.SearchField {
                id: textFilter

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
            icon.name: "view-list-tree"
            text: i18n("Scan")

            onTriggered: scanDialog.open()
        },
        Kirigami.Action {
            icon.name: "document-import"
            text: i18n("Import")

            onTriggered: importDialog.open()
        }
    ]

    ColumnLayout {
        anchors.fill: parent

        Repeater {
            model: ProjectsRegistry.loadErrors

            delegate: Kirigami.InlineMessage {
                required property string modelData

                Layout.fillWidth: true
                position: Kirigami.InlineMessage.Position.Inline
                text: `${i18n("Failed to load")} ${modelData}`
                type: Kirigami.MessageType.Error
                visible: true
            }
        }

        Controls.ScrollView {
            Controls.ScrollBar.horizontal.policy: Controls.ScrollBar.AlwaysOff
            Layout.fillHeight: true
            Layout.fillWidth: true

            Kirigami.CardsListView {
                id: projectsView

                Layout.fillWidth: true
                clip: true
                model: ProjectsRegistry.model
                reuseItems: false

                delegate: Kirigami.Card {
                    id: card

                    required property GodotProject modelData

                    banner.title: modelData.name

                    actions: [
                        Kirigami.Action {
                            id: favoriteAction

                            checkable: true
                            checked: card.modelData.favorite
                            icon.color: checked ? "gold" : "white"
                            icon.name: "favorite"

                            onCheckedChanged: card.modelData.favorite = favoriteAction.checked
                        },
                        Kirigami.Action {
                            icon.name: "document-open"
                            text: i18n("Open")
                        },
                        Kirigami.Action {
                            icon.name: "configure"
                            text: i18n("Edit")

                            onTriggered: editDialog.open()
                        },
                        Kirigami.Action {
                            icon.name: "document-open-folder"
                            text: i18n("Show in folder")
                            tooltip: card.modelData.path

                            onTriggered: card.modelData.showInFolder()
                        },
                        Kirigami.Action {
                            icon.name: "delete"
                            text: i18n("Remove")

                            onTriggered: ProjectsRegistry.remove(card.modelData)
                        }
                    ]
                    // needs two column layouts so that it's like this
                    //
                    // the first             +> +----------------------------------------------+
                    // layout sometimes      |  | text1                                        |
                    // fills the entire      |  | text2                                        |
                    // height of the         |  |       <| so the bottom item will expand to   |
                    // card                  |  |       <| fill the space it leaves            |
                    //                       +> +----------------------------------------------+
                    contentItem: ColumnLayout {
                        spacing: 0

                        ColumnLayout {
                            Controls.Label {
                                property bool hasDescription: card.modelData.description !== ""

                                Layout.fillHeight: false
                                color: hasDescription ? palette.windowText : palette.placeholderText
                                text: hasDescription ? card.modelData.description : i18n("<no description>")
                            }

                            DateLabel {
                                Layout.fillHeight: false
                                color: palette.placeholderText
                                dateTime: card.modelData.lastEditedTime
                                prefix: "Last edited "
                            }
                        }

                        // worst hack. terrible hack.
                        // ... but it works!
                        Item {
                            Layout.fillHeight: true
                        }
                    }

                    FormCard.FormCardDialog {
                        id: editDialog

                        title: card.modelData.name

                        FormCard.FormTextAreaDelegate {
                            label: i18n("Name")
                            text: card.modelData.name

                            onTextChanged: card.modelData.name = text
                        }

                        FormCard.FormTextAreaDelegate {
                            label: i18n("Description")
                            text: card.modelData.description

                            onTextChanged: card.modelData.description = text
                        }
                    }
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

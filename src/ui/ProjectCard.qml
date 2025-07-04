import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.components as KirigamiAddons
import org.kde.kirigamiaddons.formcard as FormCard

import org.fishy.godl
import org.fishy.godl.qwidgets as QWidgets

Kirigami.Card {
    id: root

    required property GodotProject modelData

    banner.title: modelData.name

    actions: [
        Kirigami.Action {
            id: favoriteAction

            checkable: true
            checked: root.modelData.favorite
            icon.color: checked ? "gold" : "white"
            icon.name: "favorite"

            onCheckedChanged: root.modelData.favorite = favoriteAction.checked
        },
        Kirigami.Action {
            icon.name: "document-export"
            text: i18n("Open")

            onTriggered: modelData.open()
        },
        Kirigami.Action {
            icon.name: "configure"
            text: i18n("Configure")

            onTriggered: editDialog.open()
        },
        Kirigami.Action {
            icon.name: "document-open-folder"
            text: i18n("Show in folder")
            tooltip: root.modelData.path

            onTriggered: root.modelData.showInFolder()
        },
        Kirigami.Action {
            icon.name: "unlock"
            text: i18n("Bind editor")

            onTriggered: bindDialog.open()
        },
        Kirigami.Action {
            icon.name: "delete"
            text: i18n("Remove")

            onTriggered: removeDialog.open()
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
                property bool hasDescription: root.modelData.description !== ""

                Layout.fillWidth: true
                color: hasDescription ? palette.windowText : palette.placeholderText
                elide: Text.ElideRight
                text: hasDescription ? root.modelData.description : i18n("<no description>")
            }

            DateLabel {
                Layout.fillWidth: true
                color: palette.placeholderText
                dateTime: root.modelData.lastEditedTime
                elide: Text.ElideRight
                prefix: "Last edited "
            }

            Controls.Label {
                id: versionLabel

                property int forceColor: 0
                property bool versionSet: root.modelData.godotVersion !== null

                Layout.fillWidth: true
                color: {
                    forceColor;
                    if (!versionSet) {
                        return palette.placeholderText;
                    } else if (!VersionRegistry.hasVersion(root.modelData.godotVersion)) {
                        return Kirigami.Theme.negativeTextColor;
                    } else {
                        return versionSet ? palette.windowText : palette.placeholderText;
                    }
                }
                elide: Text.ElideRight
                text: versionSet ? root.modelData.godotVersion + "" : i18n("<no version>")

                Connections {
                    function onHasVersionChanged() {
                        versionLabel.forceColor++;
                    }

                    target: VersionRegistry
                }
            }
        }

        // worst hack. terrible hack.
        // ... but it works!
        Item {
            Layout.fillHeight: true
        }
    }

    Kirigami.Dialog {
        id: removeDialog

        flatFooterButtons: true
        standardButtons: Kirigami.Dialog.Ok | Kirigami.Dialog.Cancel
        title: i18n("Are you sure?")
        width: Math.max(implicitFooterWidth, implicitHeaderWidth)

        contentItem: RowLayout {
            spacing: 0
            width: removeDialog.width

            Item {
                width: Kirigami.Units.largeSpacing
            }

            Controls.Label {
                Layout.fillWidth: true
                text: moveToTrash.checked ? i18n("This will move the project to the trash and remove it from the list. **Note**: it may fail to move the project to the trash in some cases.") : i18n("This will remove it from the list, but you can add the project back later")
                textFormat: Text.MarkdownText
                wrapMode: Text.Wrap
            }
        }
        customFooterActions: [
            Kirigami.Action {
                id: moveToTrash

                checkable: true
                checked: false
                icon.name: "delete"
                text: "Move to trash"
            }
        ]

        onAccepted: {
            ProjectsRegistry.remove(root.modelData, moveToTrash.checked);
        }
    }

    FormCard.FormCardDialog {
        id: editDialog

        title: `${i18n("Editing")} ${root.modelData.name}`

        FormCard.FormTextAreaDelegate {
            label: i18n("Name")
            text: root.modelData.name

            onTextChanged: root.modelData.name = text
        }

        FormCard.FormTextAreaDelegate {
            label: i18n("Description")
            text: root.modelData.description

            onTextChanged: root.modelData.description = text
        }
    }

    FormCard.FormCardDialog {
        id: bindDialog

        title: `${i18n("Binding editor for")} ${root.modelData.name}`
        width: Math.min(parent.width - Kirigami.Units.gridUnit * 2, Kirigami.Units.gridUnit * 30)

        FormCard.AbstractFormDelegate {
            background: Item {
            }
            contentItem: RowLayout {
                Kirigami.SearchField {
                    id: versionFilter

                    Layout.fillWidth: true
                }

                Kirigami.Chip {
                    id: showCsVersionOnlyChip

                    closable: false
                    icon.name: "view-filter"
                    text: i18n(".NET (Mono) versions only")
                }
            }
        }

        Repeater {
            model: VersionRegistry.model

            delegate: FormCard.FormButtonDelegate {
                id: versionButton

                property bool current: versionButton.modelData.equals(root.modelData.godotVersion)
                required property GodotVersion modelData
                property bool unfiltered: (!showCsVersionOnlyChip.checked || modelData.isMono) && (versionFilter.text === "" || (modelData + "").indexOf(versionFilter.text) !== -1)

                Layout.preferredHeight: unfiltered ? implicitHeight : 0
                clip: true
                text: modelData + ""
                trailingLogo.implicitHeight: 16
                trailingLogo.implicitWidth: 16
                trailingLogo.source: "link"
                trailingLogo.visible: current

                Behavior on Layout.preferredHeight {
                    NumberAnimation {
                        duration: Kirigami.Units.shortDuration
                        easing.type: Easing.OutCubic
                    }
                }
                background: FormCard.FormDelegateBackground {
                    id: cardBg

                    readonly property bool _isFirst: bindDialog.contentChildren[0] === control
                    readonly property bool _isLast: bindDialog.contentChildren[bindDialog.contentChildren.length - 2] === control
                    property color bg: highlighted ? Kirigami.Theme.highlightColor : Kirigami.Theme.textColor
                    property bool highlighted: versionButton.current

                    color: {
                        let colorOpacity = 0;

                        if (!control.enabled) {
                            colorOpacity = 0;
                        } else if (control.pressed) {
                            colorOpacity = 0.2;
                        } else if (control.visualFocus) {
                            colorOpacity = 0.1;
                        } else if (!Kirigami.Settings.tabletMode && control.hovered) {
                            colorOpacity = 0.07;
                        }

                        if (highlighted) {
                            colorOpacity += 0.1;
                        }

                        return Qt.rgba(bg.r, bg.g, bg.b, colorOpacity);
                    }
                    control: null
                    corners.bottomLeftRadius: _isLast ? Kirigami.Units.largeSpacing : 0
                    corners.bottomRightRadius: _isLast ? Kirigami.Units.largeSpacing : 0
                    corners.topLeftRadius: _isFirst ? Kirigami.Units.largeSpacing : 0
                    corners.topRightRadius: _isFirst ? Kirigami.Units.largeSpacing : 0

                    Timer {
                        interval: 0
                        running: true

                        onTriggered: {
                            cardBg.control = versionButton;
                        }
                    }
                }

                onClicked: root.modelData.godotVersion = modelData.boundVersion()
            }
        }
    }
}

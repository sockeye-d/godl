import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.components as KirigamiAddons
import org.kde.kirigamiaddons.formcard as FormCard
import QtQuick.Effects as Effects

import org.fishy.godl
import org.fishy.godl.qwidgets as QWidgets

Kirigami.Card {
    id: root

    property int error: -1
    required property GodotProject modelData

    signal findVersion(string source, string tag)
    signal tagSelected(string tag)

    function goFindVersion() {
        findVersion(modelData.godotVersion.repo, modelData.godotVersion.tagName);
    }

    banner.title: modelData.name
    showClickFeedback: true

    actions: [
        Kirigami.Action {
            id: favoriteAction

            checkable: true
            checked: root.modelData.favorite
            icon.color: checked ? palette.accent : palette.text
            icon.name: checked ? "favorite-favorited" : "favorite"

            onCheckedChanged: root.modelData.favorite = favoriteAction.checked
        },
        Kirigami.Action {
            icon.name: "document-export"
            text: i18n("Open")

            onTriggered: root.error = modelData.open()
        },
        Kirigami.Action {
            icon.name: "configure"
            text: i18n("Configure")

            onTriggered: editDialog.open()
        },
        Kirigami.Action {
            icon.name: "unlock"
            text: i18n("Bind editor")

            onTriggered: bindDialog.open()
        },
        Kirigami.Action {
            displayHint: Kirigami.DisplayHint.AlwaysHide
            icon.name: "document-open-folder"
            text: i18n("Show in folder")
            tooltip: root.modelData.path

            onTriggered: root.modelData.showInFolder()
        },
        Kirigami.Action {
            displayHint: Kirigami.DisplayHint.AlwaysHide
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
    contentItem: RowLayout {
        id: content

        ColumnLayout {
            id: realContent

            Layout.fillHeight: false
            Layout.fillWidth: true
            spacing: 0

            onYChanged: y = 0

            RowLayout {
                ColumnLayout {
                    Layout.fillWidth: false

                    Controls.Label {
                        property bool hasDescription: root.modelData.description !== ""

                        Layout.fillWidth: true
                        color: hasDescription ? palette.windowText : palette.placeholderText
                        elide: Text.ElideRight
                        maximumLineCount: 1
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
                        textFormat: Text.PlainText

                        Connections {
                            function onHasVersionChanged() {
                                versionLabel.forceColor++;
                            }

                            target: VersionRegistry
                        }

                        MouseArea {
                            acceptedButtons: Qt.LeftButton
                            anchors.fill: parent
                            cursorShape: enabled ? Qt.PointingHandCursor : Qt.ArrowCursor
                            enabled: {
                                versionLabel.forceColor;
                                return root.modelData.godotVersion !== null && !VersionRegistry.hasVersion(root.modelData.godotVersion);
                            }

                            onClicked: root.goFindVersion()
                        }
                    }
                }

                Kirigami.InlineMessage {
                    id: msg

                    Layout.alignment: Qt.AlignTop
                    Layout.fillWidth: true
                    Layout.preferredWidth: Kirigami.Units.gridUnit * 10
                    text: (["huh", i18n("Opened project"), i18n("No editor bound"), i18n("No editor found"), i18n("Failed to start editor")][root.modelData.lastOpenError + 1])
                    type: root.modelData.lastOpenError === 0 ? Kirigami.MessageType.Positive : Kirigami.MessageType.Error
                    visible: root.modelData.lastOpenError !== -1

                    actions: [
                        Kirigami.Action {
                            visible: root.modelData.lastOpenError === 2

                            displayComponent: Controls.ToolButton {
                                icon.name: "download"

                                onClicked: root.goFindVersion()
                            }
                        },
                        Kirigami.Action {
                            displayComponent: Controls.ToolButton {
                                icon.name: "dialog-close"

                                onClicked: root.modelData.clearError()
                            }
                        }
                    ]
                }
            }

            Item {
                id: spacer

                Layout.fillWidth: true
                height: Kirigami.Units.largeSpacing
            }

            Controls.ScrollView {
                Layout.fillWidth: true
                Layout.preferredHeight: tags.height + Controls.ScrollBar.horizontal.height + Kirigami.Units.smallSpacing
                clip: true
                contentWidth: tags.width

                RowLayout {
                    id: tags

                    property bool editing: false

                    spacing: Kirigami.Units.mediumSpacing

                    Controls.ToolButton {
                        id: addTag

                        Layout.preferredHeight: measureChip.height
                        Layout.preferredWidth: measureChip.height
                        enabled: !tags.editing || root.modelData.tags.indexOf(addTagField.editText) === -1
                        icon.name: "tag-new"
                        text: ""

                        onClicked: if (tags.editing) {
                            addTagField.accepted();
                        } else {
                            tags.editing = true;
                            addTagField.focus = true;
                        }
                    }

                    Controls.ComboBox {
                        id: addTagField

                        Layout.maximumHeight: measureChip.height
                        Layout.preferredWidth: Kirigami.Units.gridUnit * 7
                        editable: true
                        model: {
                            tags.editing;
                            return ProjectsRegistry.tags();
                        }
                        visible: tags.editing

                        onAccepted: if (addTag.enabled) {
                            tags.editing = false;
                            root.modelData.tags = root.modelData.tags.concat(editText);
                            text = "";
                        }
                    }

                    Repeater {
                        model: root.modelData.tags

                        Kirigami.Chip {
                            required property int index
                            required property string modelData

                            closable: true
                            text: modelData

                            onClicked: root.tagSelected(modelData)
                            onRemoved: {
                                let newArr = root.modelData.tags.slice();
                                newArr.splice(index, 1);
                                root.modelData.tags = newArr;
                            }
                        }
                    }

                    Kirigami.Chip {
                        id: measureChip

                        Layout.preferredWidth: 0
                        checkable: false
                        closable: false
                    }
                }
            }
        }

        // worst hack. terrible hack.
        // ... but it works!
        Item {
            Layout.fillHeight: true
        }

        Image {
            id: projectIcon

            fillMode: Image.PreserveAspectCrop
            height: realContent.height
            source: modelData.iconSource
            sourceSize.height: height
            sourceSize.width: width
            width: realContent.height

            onYChanged: y = 0

            Controls.Button {
                id: projectIconEditButton

                anchors.fill: parent
                flat: true
                icon.height: 128
                icon.name: "entry-edit"
                icon.width: 128
                opacity: projectIconArea.containsMouse ? 1 : 0

                Behavior on opacity {
                    NumberAnimation {
                        duration: Kirigami.Units.veryLongDuration
                        easing.type: Easing.OutExpo
                    }
                }

                onClicked: {
                    iconPickerFileDialog.startDirectory = root.modelData.iconDirectory;
                    iconPickerFileDialog.open();
                }
            }

            Effects.MultiEffect {
                anchors.fill: projectIconEditButton
                blurMax: 24
                blurMultiplier: 2
                opacity: projectIconEditButton.opacity
                shadowBlur: 1.0
                shadowColor: Kirigami.Theme.backgroundColor
                shadowEnabled: true
                shadowHorizontalOffset: 0
                shadowOpacity: 1.0
                shadowVerticalOffset: 10
                source: projectIconEditButton
            }

            MouseArea {
                id: projectIconArea

                acceptedButtons: Qt.NoButton
                anchors.fill: parent
                hoverEnabled: true
            }
        }
    }

    onClicked: root.error = modelData.open()

    Kirigami.Dialog {
        id: removeDialog

        flatFooterButtons: true
        standardButtons: Kirigami.Dialog.Ok | Kirigami.Dialog.Cancel
        title: i18n("Are you sure?")
        width: Kirigami.Units.gridUnit * 20

        contentItem: RowLayout {
            spacing: 0
            width: removeDialog.width

            Item {
                width: Kirigami.Units.largeSpacing
            }

            ColumnLayout {
                Layout.fillWidth: true

                Controls.Label {
                    Layout.fillWidth: true
                    text: moveToTrash.checked ? i18n("This will move the project to the trash and remove it from the list.") : i18n("This will remove it from the list, but you can add the project back later")
                    wrapMode: Text.Wrap
                }

                Controls.CheckBox {
                    id: moveToTrash

                    checked: false
                    icon.name: "delete"
                    text: i18n("Move to trash")
                }
            }

            Item {
                width: Kirigami.Units.largeSpacing
            }
        }

        // customFooterActions: [
        //     Kirigami.Action {

        //         checkable: true
        //         checked: false
        //         icon.name: "delete"
        //         text: "Move to trash"
        //     }
        // ]

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

        FormCard.FormButtonDelegate {
            text: i18n("Unbind")
            trailingLogo.implicitHeight: 16
            trailingLogo.implicitWidth: 16
            trailingLogo.source: "remove-link"

            onClicked: root.modelData.godotVersion = null
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

                        if (control === null) {
                            return Qt.transparent;
                        }

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

    BetterFileDialog {
        id: iconPickerFileDialog

        fileFilters: BetterFileDialog.NoDotAndDotDot
        filters: ["Images (*.png *.svg *.jpg *.jpeg *.jfif)"]
        mode: QWidgets.QFileDialog.ExistingFile

        onAccepted: path => {
            if (path.startsWith(root.modelData.directory)) {
                root.modelData.icon = path.slice(root.modelData.directory.length);
            } else {
                invalidIconDialog.open();
            }
        }
    }

    Kirigami.Dialog {
        id: invalidIconDialog

        padding: Kirigami.Units.largeSpacing
        standardButtons: Kirigami.Dialog.Ok
        title: i18n("Invalid icon")

        contentItem: Controls.Label {
            text: i18n("Selecting icons outside of the project directory is unsupported.")
        }
    }
}

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard

import org.fishy.godl

Kirigami.Page {
    padding: 0
    title: i18n("Local versions")

    actions: [
        Kirigami.Action {
            icon.name: "edit-clear-all"
            text: i18n("Clean leaked binaries")
            tooltip: i18n("Delete from the filesystem the binaries downloaded but not listed here")

            onTriggered: leakedVersionDialog.open()
        }
    ]

    Kirigami.Dialog {
        id: leakedVersionDialog

        property list<var> leaks

        padding: Kirigami.Units.largeSpacing
        standardButtons: leaks.length > 0 ? Controls.Dialog.Ok | Controls.Dialog.Cancel : Controls.Dialog.Ok
        title: i18n("Cleaning leaked binaries")
        width: Kirigami.Units.gridUnit * 30

        contentItem: ColumnLayout {
            Repeater {
                id: repeater

                delegate: Controls.CheckBox {
                    id: leakLayout

                    required property string confirm
                    required property int index
                    required property string path

                    Layout.fillWidth: true
                    checked: leakLayout.confirm
                    text: leakLayout.path

                    onCheckedChanged: leakedVersionDialog.leaks[index].confirm = checked
                }
            }

            Kirigami.PlaceholderMessage {
                Layout.fillHeight: true
                Layout.fillWidth: true
                // @disable-check M17
                icon.name: "edit-clear-all"
                text: i18n("No leaked versions")
                visible: repeater.count === 0
            }
        }

        onAboutToShow: {
            let l = VersionRegistry.detectLeakedVersions();
            leaks = [];
            for (let leak of l) {
                leaks.push({
                    "path": leak,
                    "confirm": true
                });
            }

            repeater.model = [];
            repeater.model = leaks;
        }
        onAccepted: if (leaks.length > 0)
            VersionRegistry.deleteLeakedVersions(leaks.filter(x => x.confirm).map(x => x.path))
    }

    Controls.ScrollView {
        Controls.ScrollBar.horizontal.policy: Controls.ScrollBar.AlwaysOff
        Layout.fillHeight: true
        Layout.fillWidth: true
        anchors.fill: parent

        GridView {
            id: versionsView

            Layout.fillWidth: true
            cellWidth: Math.floor(width / Math.max(1, Math.floor(root.width / (Kirigami.Units.gridUnit * 40))))
            clip: true
            model: VersionRegistry.model

            delegate: LocalVersionCard {
                width: versionsView.cellWidth - Kirigami.Units.largeSpacing

                onHeightChanged: versionsView.cellHeight = Math.floor(height + Kirigami.Units.largeSpacing)
            }
        }
    }

    Kirigami.PlaceholderMessage {
        anchors.centerIn: parent
        // @disable-check M17
        icon.name: "edit-none"
        text: i18n("No versions have been downloaded")
        visible: versionsView.count === 0
    }
}

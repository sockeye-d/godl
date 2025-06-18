import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard
import org.kde.kirigamiaddons.settings as KirigamiSettings

import org.fishy.godl

Kirigami.Page {
    title: i18n("General")

    FormCard.FormCard {
        id: formCard

        anchors.left: parent.left
        anchors.right: parent.right

        // Controls.Button {
        //     Layout.fillWidth: true
        //     icon.name: "list-add"
        // }
        FormCard.FormButtonDelegate {
            icon.name: "list-add"
            text: i18n("Add new filter")
            trailingLogo.visible: false

            onClicked: {
                for (let filter of Config.downloadFilter) {
                    console.log(filter);
                }

                Config.downloadFilter.push("");
            }
        }
        ColumnLayout {
            id: layout

            Layout.fillWidth: true

            // x: FormCard.FormCardUnits.horizontalPadding
            // Layout.preferredHeight: childrenRect.height + FormCard.FormCardUnits.verticalPadding * 2
            // spacing: 0

            Repeater {
                model: Config.downloadFilter

                delegate: Item {
                    id: item

                    required property int index
                    required property string modelData

                    Layout.fillWidth: true
                    height: childrenRect.height

                    Kirigami.ActionTextField {
                        text: item.modelData

                        // Layout.fillWidth: true
                        width: item.width - FormCard.FormCardUnits.horizontalPadding * 2
                        x: FormCard.FormCardUnits.horizontalPadding

                        rightActions: [
                            Kirigami.Action {
                                icon.name: "list-remove"

                                onTriggered: Config.downloadFilter.splice(item.index, 1)
                            }
                        ]

                        onTextChanged: {
                            Config.downloadFilter[item.index] = text;
                        }
                    }
                }
            }
            Item {
                height: FormCard.FormCardUnits.verticalPadding
            }
        }
    }
}

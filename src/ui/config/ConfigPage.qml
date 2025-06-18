import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard

import org.fishy.godl

Kirigami.Page {
    id: root

    required property Kirigami.PageRow pageRow

    implicitWidth: pageRow.depth <= 1 ? parent.width : Kirigami.Units.gridUnit * 10
    title: i18n("Settings")

    ColumnLayout {
        anchors.left: parent.left
        anchors.right: parent.right

        FormCard.FormCard {
            ConfigPageFormCard {
                modelData: generalPage
                pageRow: root.pageRow
            }
            ConfigPageFormCard {
                modelData: downloadsPage
                pageRow: root.pageRow
            }
        }
    }
    Kirigami.Page {
        id: generalPage

        title: i18n("General")

        FormCard.FormCard {
            anchors.left: parent.left
            anchors.right: parent.right

            FormCard.FormTextFieldDelegate {
                label: "Godot location"
                text: Config.godotLocation

                action: Kirigami.Action {
                    text: "action"
                }
            }
        }
    }
    Kirigami.Page {
        id: downloadsPage

        title: i18n("Downloads")

        FormCard.FormCard {
            anchors.left: parent.left
            anchors.right: parent.right

            RowLayout {
                Controls.Button {
                    text: "hi"
                }
            }
            FormCard.FormTextFieldDelegate {
                label: "hi"
                text: "Config.godotLocation"
            }
            // FormCard.FormTextFieldDelegate {
            //     text: Config.godotLocation
            // }
        }
    }
}

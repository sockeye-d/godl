import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard
import org.kde.kirigamiaddons.settings as KirigamiSettings

import org.fishy.godl

GodlConfigPage {
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

            onTextChanged: Config.godotLocation = text
        }
    }
}

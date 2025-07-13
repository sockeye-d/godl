import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard
import org.kde.kirigamiaddons.settings as KirigamiSettings

import org.fishy.godl

FormCard.FormCardPage {
    topPadding: Kirigami.Units.largeSpacing

    actions: [
        Kirigami.Action {
            enabled: ConfigSignals.isSaveNeeded
            icon.name: "dialog-ok-apply"
            text: i18n("Apply")
            tooltip: i18n("Commit changes to the configuration file. The changes will be saved when the application closes regardless of whether you click this button or not, but you can force it with the button.")

            onTriggered: ConfigSignals.save()
        }
    ]

    Connections {
        target: ConfigSignals
    }
}

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard
import org.kde.kirigamiaddons.settings as KirigamiSettings

import org.fishy.godl

Kirigami.Page {
    actions: [
        Kirigami.Action {
            enabled: ConfigSignals.isSaveNeeded
            icon.name: "dialog-ok-apply"
            text: i18n("Apply")

            onTriggered: ConfigSignals.save()
        }
    ]

    Connections {
        function onConfigChanged() {
            console.log("it change");
        }

        target: ConfigSignals
    }
}

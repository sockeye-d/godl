import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls
import org.kde.kirigami as Kirigami

import org.fishy.godl

Kirigami.Page {
    actions: [
        Kirigami.Action {
            icon.name: "view-refresh"
            text: i18n("Refresh")
        },
        Kirigami.Action {
            icon.name: "search"

            displayComponent: Kirigami.SearchField {
            }
        }
    ]

    Controls.Label {
        anchors.fill: parent
        horizontalAlignment: Text.AlignHCenter
        text: "projects page"
        verticalAlignment: Text.AlignVCenter
    }
}

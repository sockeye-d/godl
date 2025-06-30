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
        },
        Kirigami.Action {
            icon.name: "view-list-tree"
            text: i18n("Scan")
        },
        Kirigami.Action {
            icon.name: "document-import"
            text: i18n("Import")
        }
    ]

    Controls.Label {
        anchors.fill: parent
        horizontalAlignment: Text.AlignHCenter
        text: "projects page"
        verticalAlignment: Text.AlignVCenter
    }
}

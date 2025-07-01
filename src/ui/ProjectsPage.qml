import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls
import org.kde.kirigami as Kirigami

import org.fishy.godl
import org.fishy.godl.qwidgets as QWidgets

Kirigami.Page {
    id: root

    padding: 0

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

            onTriggered: scanDialog.open()
        },
        Kirigami.Action {
            icon.name: "document-import"
            text: i18n("Import")
        }
    ]

    Controls.ScrollView {
        Controls.ScrollBar.horizontal.policy: Controls.ScrollBar.AlwaysOff
        Layout.fillHeight: true
        Layout.fillWidth: true
        anchors.fill: parent

        Kirigami.CardsListView {
            Layout.fillWidth: true
            clip: true
            model: ProjectRegistry.model

            delegate: Kirigami.Card {
                id: card

                required property GodotProject modelData

                banner.title: modelData.name

                contentItem: Controls.Label {
                    text: card.modelData + ""
                }

                // Component.onCompleted: console.log(modelData)
            }
        }
    }

    BetterFileDialog {
        id: scanDialog

        fileFilters: BetterFileDialog.Hidden | BetterFileDialog.NoDotAndDotDot
        mode: QWidgets.FileDialog.Directory
        startDirectory: Config.projectLocation

        onAccepted: path => ProjectRegistry.scan(path)
    }
}

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard
import org.kde.kirigamiaddons.settings as KirigamiSettings

import org.fishy.godl

Kirigami.Page {
    id: root

    title: i18n("General")

    FormCard.FormCard {
        id: downloadFiltersCard

        property list<string> filters

        function refreshFilters() {
            filters = Config.downloadFilter.slice();
        }

        anchors.left: parent.left
        anchors.right: parent.right

        Component.onCompleted: {
            refreshFilters();
        }

        // Controls.Button {
        //     Layout.fillWidth: true
        //     icon.name: "list-add"
        // }
        FormCard.FormButtonDelegate {
            icon.name: "list-add"
            text: i18n("Add new filter")
            trailingLogo.visible: false

            onClicked: {
                Config.downloadFilter.push("");
                downloadFiltersCard.refreshFilters();
            }
        }
        ColumnLayout {
            id: layout

            Layout.fillWidth: true

            // x: FormCard.FormCardUnits.horizontalPadding
            // Layout.preferredHeight: childrenRect.height + FormCard.FormCardUnits.verticalPadding * 2
            // spacing: 0

            Repeater {
                model: downloadFiltersCard.filters

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

                                onTriggered: {
                                    Config.downloadFilter.splice(item.index, 1);
                                    downloadFiltersCard.refreshFilters();
                                }
                            }
                        ]

                        onTextChanged: Config.downloadFilter[item.index] = text
                    }
                }
            }
            Item {
                height: FormCard.FormCardUnits.verticalPadding
            }
        }
    }
}

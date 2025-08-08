import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard
import org.kde.kirigamiaddons.settings as KirigamiSettings

import org.fishy.godl

FormCard.FormCard {
    id: root

    property list<string> configValue
    property list<string> defaultConfigValue
    property list<Component> fieldActions: []
    property string help
    property alias text: button.text

    signal configChanged(int index, string newValue)
    signal configRemoved(int index)
    signal configSet(list<string> newValue)

    function refreshItems() {
        repeater.model = [];
        repeater.model = root.configValue.slice();
    }

    Component.onCompleted: refreshItems()

    RowLayout {
        FormCard.FormButtonDelegate {
            id: button

            Layout.fillWidth: true
            icon.name: "list-add"
            trailingLogo.visible: false

            onClicked: {
                configValue.push("");
                root.refreshItems();
            }
            onHoveredChanged: if (root.help !== "")
                tt.visible = hovered

            Controls.ToolTip {
                id: tt

                text: root.help
            }
        }

        FormCard.FormButtonDelegate {
            Layout.fillWidth: false
            enabled: JSON.stringify(configValue) !== JSON.stringify(defaultConfigValue)
            icon.name: "document-revert"
            text: i18n("Reset")
            trailingLogo.visible: false

            onClicked: {
                root.configSet(defaultConfigValue);
                root.refreshItems();
            }
        }
    }

    ColumnLayout {
        Layout.fillWidth: true

        Repeater {
            id: repeater

            Item {
                id: item

                required property int index
                required property string modelData

                Layout.fillWidth: true
                height: childrenRect.height

                Kirigami.ActionTextField {
                    id: textField

                    property var defaultAction: Kirigami.Action {
                        icon.name: "list-remove"

                        onTriggered: {
                            root.configRemoved(item.index);
                            root.refreshItems();
                        }
                    }

                    rightActions: {
                        let actions = [defaultAction];
                        for (let action of root.fieldActions) {
                            actions.push(action.createObject(null, {
                                index: item.index,
                                data: item.modelData
                            }));
                        }
                        return actions;
                    }
                    text: item.modelData
                    width: item.width - FormCard.FormCardUnits.horizontalPadding * 2
                    x: FormCard.FormCardUnits.horizontalPadding

                    onTextChanged: root.configChanged(index, text)
                }
            }
        }

        Item {
            height: FormCard.FormCardUnits.verticalPadding
        }
    }
}

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard
import org.kde.kirigamiaddons.settings as KirigamiSettings

FormCard.FormTextFieldDelegate {
    id: root

    property alias leftActions: textField.leftActions
    property alias rightActions: textField.rightActions

    contentItem: ColumnLayout {
        spacing: FormCard.FormCardUnits.verticalSpacing

        RowLayout {
            Layout.fillWidth: true
            spacing: FormCard.FormCardUnits.horizontalSpacing

            Label {
                Accessible.ignored: true
                Layout.fillWidth: true
                color: root.enabled ? Kirigami.Theme.textColor : Kirigami.Theme.disabledTextColor
                elide: Text.ElideRight
                maximumLineCount: 2
                text: label
                wrapMode: Text.Wrap
            }
            Label {
                Accessible.ignored: !visible
                Layout.preferredWidth: metrics.advanceWidth
                color: textField.text.length === root.maximumLength ? Kirigami.Theme.neutralTextColor : Kirigami.Theme.textColor
                font: Kirigami.Theme.smallFont
                horizontalAlignment: Text.AlignRight
                text: metrics.label(textField.text.length, root.maximumLength)
                // 32767 is the default value for TextField.maximumLength
                visible: root.maximumLength < 32767

                TextMetrics {
                    id: metrics

                    function label(current: int, maximum: int): string {
                        return i18ndc("kirigami-addons6", "@label %1 is current text length, %2 is maximum length of text field", "%1/%2", current, maximum);
                    }

                    font: Kirigami.Theme.smallFont
                    text: label(root.maximumLength, root.maximumLength)
                }
            }
        }
        RowLayout {
            id: innerRow

            Layout.fillWidth: true
            spacing: Kirigami.Units.smallSpacing

            Kirigami.ActionTextField {
                id: textField

                Accessible.name: root.label
                Layout.fillWidth: true
                activeFocusOnTab: false
                placeholderText: root.placeholderText
                text: root.text

                onAccepted: root.accepted()
                onEditingFinished: root.editingFinished()
                onTextChanged: root.text = text
                onTextEdited: root.textEdited()
            }
        }
        Kirigami.InlineMessage {
            id: formErrorHandler

            Layout.fillWidth: true
            Layout.topMargin: visible ? Kirigami.Units.smallSpacing : 0
            text: root.statusMessage
            type: root.status
            visible: root.statusMessage.length > 0
        }
    }
}

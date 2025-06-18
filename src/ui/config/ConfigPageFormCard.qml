import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard

FormCard.FormButtonDelegate {
    id: root

    required property Kirigami.Page modelData
    required property Kirigami.PageRow pageRow

    // enabled: pageRow.currentItem !== settingsPage
    text: modelData.title
    trailingLogo.direction: pageRow.lastItem === modelData ? Qt.LeftArrow : Qt.RightArrow

    background: FormCard.FormDelegateBackground {
        color: {
            let colorOpacity = 0;

            if (!control.enabled) {
                colorOpacity = 0;
            } else if (control.pressed) {
                colorOpacity = 0.2;
            } else if (control.visualFocus) {
                colorOpacity = 0.1;
            } else if (!Kirigami.Settings.tabletMode && control.hovered || pageRow.lastItem === modelData) {
                colorOpacity = 0.07;
            }

            return Qt.rgba(Kirigami.Theme.textColor.r, Kirigami.Theme.textColor.g, Kirigami.Theme.textColor.b, colorOpacity);
        }
        control: root
    }

    onClicked: if (pageRow.lastItem === modelData)
        pageRow.pop()
    else if (pageRow.depth > 1)
        pageRow.replace(modelData)
    else
        pageRow.push(modelData)
}

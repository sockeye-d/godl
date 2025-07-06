import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.components as KirigamiAddons
import org.kde.kirigamiaddons.formcard as FormCard

import org.fishy.godl
import org.fishy.godl.qwidgets as QWidgets

Kirigami.ScrollablePage {
    title: i18n("New project")
    Kirigami.FormLayout {
        anchors.fill: parent
        Item {
            Layout.preferredWidth: Kirigami.Units.gridUnit * 30
        }

        Item {
            Kirigami.FormData.label: i18n("Project metadata")
            Kirigami.FormData.isSection: true
        }

        Controls.TextField {
            id: formName
            Kirigami.FormData.label: i18n("Name")
        }

        Controls.TextField {
            Kirigami.FormData.label: i18n("Description")
        }

        Item {
            Kirigami.FormData.label: i18n("Project settings")
            Kirigami.FormData.isSection: true
        }

        Controls.ComboBox {
            id: rendererComboBox
            Layout.fillWidth: true
            model: ["Forward+", "Mobile", "Compatibility",]

            Kirigami.FormData.label: i18n("Renderer")
        }

        Item {
            Kirigami.FormData.label: i18n("Other")
            Kirigami.FormData.isSection: true
        }

        Controls.ComboBox {
            model: ["None", "Git"]
            currentIndex: 1
            Layout.fillWidth: true
            Kirigami.FormData.label: i18n("Version control system")
        }

        Controls.TextField {
            Kirigami.FormData.label: i18n("Project location")
            placeholderText: Config.projectLocation + "/" + formName.text
        }

        Controls.Button {
            Layout.fillWidth: true
            text: i18n("Create project")
        }
    }
}

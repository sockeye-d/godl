import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.components as KirigamiAddons
import org.kde.kirigamiaddons.formcard as FormCard

import org.fishy.godl
import org.fishy.godl.qwidgets as QWidgets

Kirigami.ScrollablePage {
    id: root

    property projectTemplate activeTemplate: ProjectTemplates.templ(templateSelector.currentValue)
    property var configComponents: {
        "string": stringComponent,
        "enum": enumComponent,
        "header": headerComponent
    }

    title: i18n("New project")

    actions: [
        Kirigami.Action {
            icon.color: Kirigami.Theme.positiveTextColor
            icon.name: "document-new-from-template"
            text: i18n("Create project")
        }
    ]

    Component.onCompleted: ProjectTemplates.rescan()

    Kirigami.FormLayout {
        id: form

        Layout.fillHeight: false
        Layout.fillWidth: true

        Item {
            Layout.preferredWidth: Kirigami.Units.gridUnit * 30
        }

        Controls.ComboBox {
            id: templateSelector

            Kirigami.FormData.label: i18n("Template")
            Layout.fillWidth: true
            model: ProjectTemplates.templates
        }

        Controls.TextField {
            id: formName

            Kirigami.FormData.label: i18n("Name")
        }

        Controls.TextField {
            Kirigami.FormData.label: i18n("Project location")
            placeholderText: Configuration.projectLocation + "/" + formName.text
        }

        Repeater {
            model: JSON.parse(root.activeTemplate.meta)

            delegate: Item {
                property Item component
                required property string label
                required property var modelData
                required property string type

                Kirigami.FormData.isSection: type == "header"
                Kirigami.FormData.label: label
                Layout.fillWidth: true
                Layout.preferredHeight: component?.height
                implicitHeight: component?.implicitHeight

                Component.onCompleted: component = root.configComponents[type].createObject(this, modelData)
                onWidthChanged: component.width = width
            }
        }
    }

    Component {
        id: stringComponent

        Controls.TextField {
            required property string label
            required property string template
            required property string type
        }
    }

    Component {
        id: headerComponent

        Item {
            required property string label
            required property string type
        }
    }

    Component {
        id: enumComponent

        Controls.ComboBox {
            required property string label
            required property string template
            required property string type
            required property list<var> values

            model: values
            textRole: "label"
            valueRole: "key"
        }
    }
}

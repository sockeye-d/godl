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
        "multistring": multistringComponent,
        "enum": enumComponent,
        "header": headerComponent
    }
    property var replacements

    signal setupReplacements

    function createProject() {
        replacements = {
            "name": formName.text
        };
        setupReplacements();
        ProjectTemplates.createProject(templateSelector.currentValue, projectLocation.currentText, replacements);
        ProjectsRegistry.import(projectLocation.currentText);
        ProjectsRegistry.model.filter = formName.text;
        applicationWindow().pageStack.layers.pop();
    }

    title: i18n("New project")

    actions: [
        Kirigami.Action {
            enabled: ProjectTemplates.isProjectValid(projectLocation.currentText)
            icon.color: Kirigami.Theme.positiveTextColor
            icon.name: "document-new-from-template"
            text: i18n("Create project")

            onTriggered: root.createProject()
        }
    ]

    Component.onCompleted: {
        ProjectTemplates.rescan();
        templateSelector.currentIndex = ProjectTemplates.templates.indexOf(Configuration.defaultTemplate);
    }

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

        RowLayout {
            Kirigami.FormData.label: i18n("Name")
            Layout.fillWidth: true

            Controls.TextField {
                id: formName

                Layout.fillWidth: true
            }

            Controls.ToolButton {
                icon.name: "randomize"

                onClicked: formName.text = ProjectTemplates.generateRandomName()
            }
        }

        Controls.TextField {
            id: projectLocation

            property string currentText: text === "" ? placeholderText : text

            Kirigami.FormData.label: i18n("Project location")
            placeholderText: Configuration.projectLocation + "/" + formName.text
        }

        Repeater {
            model: JSON.parse(root.activeTemplate.meta).replacements

            delegate: Item {
                id: item

                property Item component
                required property string label
                required property var modelData
                required property string type

                Kirigami.FormData.isSection: type == "header"
                Kirigami.FormData.label: label
                Layout.fillWidth: true
                Layout.preferredHeight: component?.height
                implicitHeight: component?.implicitHeight

                Component.onCompleted: {
                    let passedData = JSON.parse(JSON.stringify(modelData));
                    delete passedData.label;
                    delete passedData.type;
                    delete passedData.template;
                    component = root.configComponents[type].createObject(this, passedData);
                }
                onWidthChanged: component.width = width

                Connections {
                    function onSetupReplacements() {
                        if (item.modelData.template !== undefined && item.modelData.template !== null && 'replacement' in item.component) {
                            root.replacements[item.modelData.template] = item.component.replacement;
                        }
                    }

                    target: root
                }
            }
        }
    }

    Component {
        id: stringComponent

        Controls.TextField {
            property string replacement: text
        }
    }

    Component {
        id: multistringComponent

        Controls.TextArea {
            property string replacement: text
        }
    }

    Component {
        id: headerComponent

        Item {
        }
    }

    Component {
        id: enumComponent

        Controls.ComboBox {
            property string replacement: currentValue
            required property list<var> values

            model: values
            textRole: "label"
            valueRole: "key"
        }
    }
}

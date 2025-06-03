// Includes relevant modules used by the QML
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls
import org.kde.kirigami as Kirigami

import org.fishy.godl 0.1

// Provides basic features needed for all kirigami applications
Kirigami.ApplicationWindow {
    // Unique identifier to reference this object
    id: root

    width: 400
    height: 300

    // Window title
    // i18nc() makes a string translatable
    // and provides additional context for the translators
    title: i18nc("@title:window", "Hello World")

    globalDrawer: Kirigami.GlobalDrawer {
        isMenu: true
        actions: [
            Kirigami.Action {
                text: i18n("About")
                icon.name: "help-about"
                onTriggered: pageStack.layers.push(aboutPage)
            }
        ]
    }

    Component {
        id: aboutPage

        Kirigami.AboutPage {
            aboutData: About
        }
    }

    ChainedJsonRequest {
        id: request

        onFinished: result => {
                        console.log(JSON.stringify(result, null, 4))
                    }

        Component.onCompleted: {
            request.add(result => result.map(x => x.author.url))
        }
    }

    // Set the first page that will be loaded when the app opens
    // This can also be set to an id of a Kirigami.Page
    pageStack.initialPage: Kirigami.Page {
        Row {
            anchors.fill: parent
            Controls.Button {
                text: "Hello!"
                onClicked: {
                    let urls = [Qt.url(
                                    "https://api.github.com/repos/godotengine/godot/releases?per_page=100")]
                    request.execute(urls)
                }
            }
            ListView {}
        }
    }
}

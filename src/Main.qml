// Includes relevant modules used by the QML
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls
import org.kde.kirigami as Kirigami

import org.fishy.godl

// Provides basic features needed for all kirigami applications
Kirigami.ApplicationWindow {
    // Unique identifier to reference this object
    id: root

    property string text

    width: 800
    height: 600

    // Window title
    // i18nc() makes a string translatable
    // and provides additional context for the translators
    title: i18nc("@title:window", "Hello World")

    globalDrawer: Kirigami.GlobalDrawer {
        isMenu: false
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

    DownloadManager {
        id: dl
    }

    pageStack.initialPage: RemoteVersionsPage {
        id: dlPage
        Component.onCompleted: {
            dlPage.dl = dl
            dlPage.pageStack = pageStack
        }
    }
}

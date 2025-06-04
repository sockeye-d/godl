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

    width: 800
    height: 600

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

        property var lastResult

        // onLastResultChanged: resultList.update()
        onFinished: result => {
                        lastResult = result[0]
                        console.debug("got result")
                        console.debug(JSON.stringify(lastResult, null, 4))
                        resultModel.clear()
                        for (let r of lastResult) {
                            resultModel.append({
                                                   "url": r.assets[0].browser_download_url
                                                   // "a": r.assets[0].browser_download_url
                                               })
                        }
                    }

        Component.onCompleted: {

            // request.add(result => result.map(x => x.author.url))
        }
    }

    DownloadManager {
        id: dl
    }

    pageStack.initialPage: Kirigami.Page {
        ColumnLayout {
            anchors.fill: parent
            Controls.Button {
                Layout.fillWidth: true
                text: "do fetch stuff"
                onClicked: {
                    // let urls = [Qt.url(
                    //                 "https://api.github.com/repos/godotengine/godot/releases?per_page=10")]
                    // request.execute(urls)
                    dl.download(Qt.url("https://api.github.com/"))
                }
            }
            Controls.ScrollView {
                Layout.fillWidth: true
                Layout.fillHeight: true

                // contentWidth: availableWidth - 100
                Controls.ScrollBar.horizontal.policy: Controls.ScrollBar.AlwaysOff
                Kirigami.CardsListView {
                    id: resultList
                    clip: true
                    model: ListModel {
                        id: resultModel
                    }

                    delegate: Kirigami.Card {
                        required property string url

                        // width: resultList.width
                        // headerOrientation: Qt.Horizontal
                        actions: [
                            Kirigami.Action {
                                text: i18n("Download")
                                icon.name: "download"
                            },
                            Kirigami.Action {
                                text: i18n("Open")
                                icon.name: "link"
                            }
                        ]

                        banner {
                            title: url
                        }

                        contentItem: Controls.Label {
                            text: url
                        }
                    }
                }
            }
        }
    }
}

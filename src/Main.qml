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

    Component.onCompleted: {
        console.log(Config.godotLocation)
    }

    Component {
        id: aboutPage

        Kirigami.AboutPage {
            aboutData: About
        }
    }

    Component {
        id: resultPage
        Kirigami.Page {
            Controls.ScrollView {
                anchors.fill: parent
                Controls.Label {
                    anchors.fill: parent
                    id: resultPageText
                }
            }

            Component.onCompleted: {
                resultPageText.text = root.text
            }
        }
    }

    ChainedJsonRequest {
        id: request

        property var lastResult

        function findTagName(release) {
            for (let r of release.assets) {
                if ('tag_name' in r) {
                    return r.tag_name
                }
            }
            return 'unknown'
        }

        // onLastResultChanged: resultList.update()
        onFinished: result => {
                        lastResult = result[0]
                        console.log("got result")
                        // console.log(JSON.stringify(result, null, 4))
                        root.text = JSON.stringify(result, null, 4)
                        resultModel.clear()
                        for (let r of lastResult) {
                            // let tagName = findTagName(r)
                            resultModel.append({
                                                   "url": r.html_url,
                                                   "description": r.body,
                                                   "tagName": r.tag_name
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
            RowLayout {
                Controls.Button {
                    Layout.fillWidth: true
                    text: "do fetch stuff"
                    onClicked: {
                        request.execute(
                                    [Qt.url(
                                         "https://api.github.com/repos/godotengine/godot/releases?per_page=100")])
                        // dl.download(Qt.url("https://api.github.com/"))
                    }
                }

                Controls.Button {
                    Layout.fillWidth: true
                    text: "show result text"
                    onClicked: {
                        pageStack.layers.push(resultPage)
                    }
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
                        required property string description
                        required property string tagName

                        actions: [
                            Kirigami.Action {
                                text: i18n("Download")
                                icon.name: "download"
                            },
                            Kirigami.Action {
                                text: i18n("Open")
                                icon.name: "link"
                                onTriggered: Qt.openUrlExternally(Qt.url(url))
                            }
                        ]

                        banner {
                            title: tagName
                        }

                        contentItem: Controls.Label {
                            text: description
                            wrapMode: Text.Wrap
                            textFormat: Text.MarkdownText

                            onLinkActivated: link => {
                                                 Qt.openUrlExternally(
                                                     Qt.url(link))
                                             }
                        }
                    }
                }
            }
        }
    }
}

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls
import org.kde.kirigami as Kirigami

import org.fishy.godl

Kirigami.Page {
    property DownloadManager dl
    property Kirigami.PageStack pageStack

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

        onFinished: result => {
                        lastResult = result[0]
                        console.log("got result")

                        resultModel.recompute()
                        progress.visible = false
                    }

        Component.onCompleted: {

            // request.add(result => result.map(x => x.author.url))
        }
    }

    ColumnLayout {
        anchors.fill: parent
        RowLayout {
            Controls.Button {
                Layout.fillWidth: true
                text: "do fetch stuff"
                onClicked: {
                    progress.visible = true
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

        Controls.TextField {
            id: filter
            Layout.fillWidth: true
            placeholderText: i18n("Filter...")
            onTextChanged: textFieldDebouncer.restart()

            Timer {
                id: textFieldDebouncer
                interval: 500
                onTriggered: resultModel.recompute()
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

                    function recompute() {
                        clear()
                        for (let r of request.lastResult) {
                            let obj = {
                                "url": r.html_url,
                                "description": r.body,
                                "tagName": r.tag_name
                                // "assets": r.assets
                            }
                            if (filter.text !== ""
                                    && (obj.description.indexOf(
                                            filter.text) === -1
                                        || obj.tagName.indexOf(
                                            filter.text) === -1)) {
                                continue
                            }

                            append(obj)
                        }
                    }
                }

                delegate: Kirigami.Card {
                    required property string url
                    required property string description
                    required property string tagName

                    // required property var assets
                    actions: [
                        Kirigami.Action {
                            text: i18n("Download")
                            icon.name: "download"
                            // onTriggered: dl.download(Qt.url(downloadUrl))
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
                                             Qt.openUrlExternally(Qt.url(link))
                                         }
                    }
                }
            }
        }

        Controls.ProgressBar {
            id: progress
            Layout.fillWidth: true
            Layout.fillHeight: true
            indeterminate: true
            visible: false
        }
    }
}

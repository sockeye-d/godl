import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls
import org.kde.kirigami as Kirigami

import org.fishy.godl

Kirigami.Page {
    id: root
    property DownloadManager dl
    property int requestCount: 100
    property int currentPage: 0

    ChainedJsonRequest {
        id: request

        property var lastResult
        property list<var> releases
        property string errorString: ""
        property int totalPages: -1

        onFinished: result => {
                        lastResult = result[0]
                        console.log("got result")
                        currentPage = 0
                        resultList.fullReleases = releases
                    }
        onError: (_, error, _errorString) => {
                     errorString = `${NetworkResponseCode.error(
                         error)}: ${_errorString}`
                 }

        Component.onCompleted: {
            for (var i = 0; i < 1000; i++) {
                add((r, headers) => {
                        let lastPageRegex = /page=(\d+)>; rel="last"/
                        if (totalPages === -1) {
                            let result = lastPageRegex.exec(headers.link)
                            if (result) {
                                totalPages = result[1]
                                console.log("detected total page as",
                                            totalPages)
                            } else {
                                console.log("not found :(")
                            }
                        }

                        releases.push(...r)

                        if (r.length === requestCount) {
                            currentPage++
                            console.log("requesting page", currentPage + 1)
                            return [Qt.url(
                                        `https://api.github.com/repos/godotengine/godot-builds/releases?per_page=${requestCount}&page=${currentPage + 1}`)]
                        } else {
                            return []
                        }
                    })
            }
        }
    }

    Component.onCompleted: refresh()

    function refresh() {
        request.releases.length = 0
        request.errorString = ""
        currentPage = 0
        resultList.fullReleases = []
        request.execute(
                    [Qt.url(
                         `https://api.github.com/repos/godotengine/godot-builds/releases?per_page=${requestCount}`)])
    }

    Kirigami.OverlaySheet {
        id: dlDialog

        Kirigami.CardsListView {
            id: dlDialogModel

            delegate: Kirigami.Card {
                required property string name
                required property string url
                required property int size

                banner.title: name
                contentItem: Controls.Label {
                    text: url
                    elide: Text.ElideLeft
                }
                actions: [
                    Kirigami.Action {
                        text: i18n("Download")
                        icon.name: "download"
                    }
                ]
            }
        }
    }

    ColumnLayout {
        anchors.fill: parent
        RowLayout {
            Controls.Button {
                icon.name: "view-refresh"
                text: i18n("Refresh")
                onClicked: root.refresh()
            }

            Controls.TextField {
                id: filter
                property string debouncedText: text
                Layout.fillWidth: true
                placeholderText: i18n("Filter")
                onTextChanged: textFieldDebouncer.restart()

                Timer {
                    id: textFieldDebouncer
                    interval: 100 /* ms */
                    onTriggered: parent.debouncedText = parent.text
                }
            }
        }

        Controls.ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true

            Controls.ScrollBar.horizontal.policy: Controls.ScrollBar.AlwaysOff
            Kirigami.CardsListView {
                id: resultList
                property list<var> fullReleases
                clip: true
                model: fullReleases.filter(el => filter.debouncedText === ""
                                           || el.tag_name.indexOf(
                                               filter.text) !== -1)
                delegate: Kirigami.Card {
                    required property string body
                    required property string tag_name
                    required property var assets
                    required property string created_at
                    required property string html_url

                    actions: [
                        Kirigami.Action {
                            text: i18n("Download")
                            icon.name: "download"
                            onTriggered: {
                                dlDialog.open()
                                dlDialogModel.model = assets
                            }
                        },
                        Kirigami.Action {
                            text: i18n("Open")
                            icon.name: "link"
                            onTriggered: Qt.openUrlExternally(Qt.url(html_url))
                        }
                    ]

                    banner.title: tag_name

                    contentItem: Controls.Label {
                        text: new Date(created_at).toLocaleString(
                                  ) + "\n\n" + body
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
            // property real currentValue: currentPage
            to: request.totalPages
            value: currentPage
            Layout.fillWidth: true
            Layout.fillHeight: true
            indeterminate: request.totalPages === -1
            visible: request.running

            Behavior on value {
                NumberAnimation {
                    duration: 500
                    easing.type: Easing.OutExpo
                }
            }
        }

        Kirigami.Heading {
            level: 1
            text: request.errorString
            Layout.fillWidth: true
            Layout.fillHeight: true
            visible: request.errorString !== ""
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            wrapMode: Text.Wrap
        }

        Kirigami.Heading {
            level: 1
            text: i18n("No results found 😢")
            Layout.fillWidth: true
            Layout.fillHeight: true
            visible: resultList.model.length === 0 && !request.running
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            wrapMode: Text.Wrap
        }
    }
    // onCurrentPageChanged: progress.value = currentPage
}

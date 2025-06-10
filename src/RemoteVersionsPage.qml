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
                        resultModel.recompute()
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

                        for (let item of r) {
                            releases.push(item)
                        }
                        resultModel.recompute()
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
        resultModel.clear()
        request.releases.length = 0
        request.errorString = ""
        currentPage = 0
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
                Layout.fillWidth: true
                placeholderText: i18n("Filter")
                onTextChanged: textFieldDebouncer.restart()

                Timer {
                    id: textFieldDebouncer
                    interval: 100 /* ms */
                    onTriggered: resultModel.recompute()
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

                    function recompute() {
                        clear()
                        for (let r of request.releases) {
                            let obj = {
                                "url": r.html_url,
                                "description": r.body,
                                "tagName": r.tag_name,
                                "authorProfilePicture": r.author.avatar_url,
                                "assets": r.assets.map(asset => ({
                                                                     "name": asset.name,
                                                                     "url": asset.browser_download_url,
                                                                     "size": asset.size
                                                                 })),
                                "date": new Date(r.created_at)
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
                    required property string authorProfilePicture
                    required property var assets
                    required property date date

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
                            onTriggered: Qt.openUrlExternally(Qt.url(url))
                        }
                    ]

                    banner.title: tagName

                    contentItem: Controls.Label {
                        text: date.toLocaleString() + "\n\n" + description
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
            to: request.totalPages
            value: currentPage
            Layout.fillWidth: true
            Layout.fillHeight: true
            indeterminate: request.totalPages === -1
            visible: request.running
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
    }
}

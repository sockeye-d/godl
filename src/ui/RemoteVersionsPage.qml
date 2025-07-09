import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls
import org.kde.kirigami as Kirigami

import org.fishy.godl

Kirigami.Page {
    id: root

    property DownloadManager dl
    property bool hasContent: false
    property string rawRepo: Config.sources[0]
    property int requestCount: 100
    property bool show_alpha: true
    property bool show_beta: true
    property bool show_dev: true
    property bool show_rc: true
    property bool show_stable: true

    function refresh() {
        if (request.running) {
            return;
        }

        request.releases.length = 0;
        request.errorString = "";
        request.currentPage = 0;
        request.totalPages = -1;
        resultList.fullReleases = [];
        let source = root.rawRepo;
        if (source[0] === "/") {
            source = `https://api.github.com/repos${source}`;
        }

        source = `${VersionRegistry.resolveSourceUrl(root.rawRepo)}/releases?per_page=${requestCount}`;
        console.log(source);
        request.execute([Qt.url(source)]);
    }

    padding: 0

    actions: [
        Kirigami.Action {
            enabled: !request.running
            icon.name: "view-refresh"
            tooltip: i18n("Refresh")

            onTriggered: root.refresh()
        },
        Kirigami.Action {
            displayComponent: filterComponent
            icon.name: "search"
        },
        Kirigami.Action {
            icon.name: "view-filter"
            tooltip: i18n("Filters")

            Kirigami.Action {
                checkable: true
                checked: show_stable
                text: i18n("Show stable")

                onTriggered: show_stable = checked
            }

            Kirigami.Action {
                checkable: true
                text: i18n("Show unstable")

                Binding on checked {
                    delayed: true
                    value: show_dev || show_alpha || show_beta || show_rc
                }

                onTriggered: {
                    show_dev = checked;
                    show_alpha = checked;
                    show_beta = checked;
                    show_rc = checked;
                }
            }

            Kirigami.Action {
                checkable: true
                checked: show_dev
                text: i18n("Show dev snapshots")

                onTriggered: show_dev = checked
            }

            Kirigami.Action {
                checkable: true
                checked: show_alpha
                text: i18n("Show alphas")

                onTriggered: show_alpha = checked
            }

            Kirigami.Action {
                checkable: true
                checked: show_beta
                text: i18n("Show betas")

                onTriggered: show_beta = checked
            }

            Kirigami.Action {
                checkable: true
                checked: show_rc
                text: i18n("Show release candidates")

                onTriggered: show_rc = checked
            }
        },
        Kirigami.Action {
            displayComponent: Component {
                id: dlSourceComponent

                Controls.ComboBox {
                    enabled: !request.running
                    model: Config.sources

                    onCurrentValueChanged: {
                        root.rawRepo = currentValue;
                        root.refresh();
                    }
                }
            }
        }
    ]

    Component {
        id: filterComponent

        Kirigami.SearchField {
            id: filter

            Layout.fillWidth: true

            onTextChanged: {
                resultList.filterText = text;
            }
        }
    }

    ChainedJsonRequest {
        id: request

        property int currentPage: 0
        property string errorString: ""
        property var lastResult
        property list<var> releases
        property int totalPages: -1

        Component.onCompleted: {
            for (var i = 0; i < 1000; i++) {
                add((r, headers) => {
                    let lastPageRegex = /page=(\d+)>; rel="last"/;
                    if (totalPages === -1) {
                        let result = lastPageRegex.exec(headers.link);
                        if (result) {
                            totalPages = result[1];
                        }
                    }

                    releases.push(...r);
                    resultList.fullReleases = releases.slice();

                    if (r.length === requestCount) {
                        currentPage++;
                        return [Qt.url(`https://api.github.com/repos/godotengine/godot-builds/releases?per_page=${requestCount}&page=${currentPage + 1}`)];
                    } else {
                        return [];
                    }
                });
            }
        }
        onError: (_, error, _errorString) => {
            errorString = `${NetworkResponseCode.error(error)}: ${_errorString}`;
        }
        onFinished: result => {
            lastResult = result[0];
            console.log("got result");
            currentPage = 0;
            resultList.fullReleases = releases;
            hasContent = true;
        }
    }

    Kirigami.OverlaySheet {
        id: patchNotesSheet

        property string body
        property string created_at

        // contentHeight: label.implicitHeight + Kirigami.Units.gridUnit * 10
        // height: root.height
        // padding: Kirigami.Units.largeSpacing
        // width: root.width
        // y: root.padding
        implicitHeight: root.height - y

        Kirigami.SelectableLabel {
            id: label

            Layout.fillWidth: true

            // clip: true
            // Layout.fillHeight: true
            bottomPadding: Kirigami.Units.largeSpacing
            text: new Date(patchNotesSheet.created_at).toLocaleString() + "\n\n" + patchNotesSheet.body
            textFormat: Text.MarkdownText
            visible: true
            wrapMode: Text.Wrap

            onLinkActivated: link => {
                Qt.openUrlExternally(Qt.url(link));
            }
        }
    }

    Kirigami.OverlaySheet {
        id: dlDialog

        property var assets: []
        property string tagName

        header: RowLayout {
            Kirigami.Heading {
                level: 1
                text: dlDialog.title
            }

            Kirigami.SearchField {
                id: assetsFilter

                Layout.fillWidth: true
            }

            Kirigami.Chip {
                id: currentPlatformOnlyFilterChip

                Layout.fillHeight: true
                checkable: true
                checked: true
                closable: false
                icon.name: "view-filter"
                text: i18n("Current platform only")
            }

            Kirigami.Chip {
                id: monoOnlyFilterChip

                Layout.fillHeight: true
                checkable: true
                closable: false
                icon.name: "view-filter"
                text: i18n(".NET (Mono) versions only")
            }
        }

        Kirigami.CardsListView {
            id: dlDialogModel

            property list<int> highlightedIndices: []

            function dotnetFilter(el) {
                return !monoOnlyFilterChip.checked || el.name.indexOf("mono") !== -1;
            }

            function filterAsset(el) {
                return Config.downloadFilter.some(e => el.name.indexOf(e) !== -1);
            }

            function nameFilter(el) {
                return assetsFilter.text === "" || el.name.indexOf(assetsFilter.text) !== -1;
            }

            function platformFilter(el) {
                return !currentPlatformOnlyFilterChip.checked || filterAsset(el);
            }

            model: dlDialog.assets.filter(platformFilter).filter(nameFilter).filter(dotnetFilter)

            delegate: Kirigami.Card {
                required property string browser_download_url
                required property int index
                required property string name
                required property int size
                required property string url

                banner.title: name

                actions: [
                    Kirigami.Action {
                        icon.name: "download"
                        text: i18n("Download")

                        onTriggered: {
                            // dlDialog.close()
                            dl.download(name, dlDialog.tagName, Qt.url(browser_download_url), root.rawRepo);
                        }
                    }
                ]
                contentItem: Controls.Label {
                    elide: Text.ElideLeft
                    text: browser_download_url
                }

                Rectangle {
                    anchors.fill: parent
                    border.color: Kirigami.Theme.highlightColor
                    border.width: 2
                    color: "transparent"
                    radius: parent.background.radius
                    visible: dlDialogModel.highlightedIndices.includes(parent.index)
                }
            }

            onModelChanged: {
                highlightedIndices = [];
                let i = 0;
                for (let element of model) {
                    if (filterAsset(element)) {
                        highlightedIndices.push(i);
                    }
                    i++;
                }
            }
        }
    }

    ColumnLayout {
        anchors.fill: parent

        Controls.ScrollView {
            id: scrollview

            property bool vScrollbarVisible: Controls.ScrollBar.vertical.visible

            Controls.ScrollBar.horizontal.policy: Controls.ScrollBar.AlwaysOff
            Layout.fillHeight: true
            Layout.fillWidth: true
            padding: 0

            Kirigami.CardsListView {
                id: resultList

                property string filterText
                property list<var> fullReleases

                function getFilter(filter) {
                    return e => root[`show_${filter}`] || e.tag_name.indexOf(filter) === -1;
                }

                Layout.fillWidth: true
                // cacheBuffer: 100000
                // cellWidth: width / Math.max(1, Math.round(root.width / (Kirigami.Units.gridUnit * 30)))
                clip: true
                leftMargin: 0
                model: fullReleases.filter(el => filterText === "" || el.tag_name.indexOf(filterText) !== -1).filter(getFilter("stable")).filter(getFilter("dev")).filter(getFilter("alpha")).filter(getFilter("beta")).filter(getFilter("rc"))
                rightMargin: scrollview.vScrollbarVisible ? Kirigami.Units.largeSpacing * 2 : 0
                topMargin: 0

                delegate: Kirigami.Card {
                    id: card

                    required property var assets
                    required property string body
                    required property string created_at
                    required property string html_url
                    required property string tag_name

                    banner.title: tag_name

                    // height: resultList.cellHeight - Kirigami.Units.largeSpacing
                    // width: resultList.cellWidth - Kirigami.Units.largeSpacing
                    actions: [
                        Kirigami.Action {
                            icon.name: "document-preview"
                            text: i18n("Show patch notes")

                            onTriggered: {
                                patchNotesSheet.title = card.tag_name;
                                patchNotesSheet.body = card.body;
                                patchNotesSheet.created_at = card.created_at;
                                patchNotesSheet.open();
                            }
                        },
                        Kirigami.Action {
                            icon.name: "download"
                            text: i18n("Download")

                            onTriggered: {
                                dlDialog.assets = assets;
                                dlDialog.title = `${i18n("Assets for")} ${tag_name}`;
                                dlDialog.tagName = tag_name;
                                dlDialog.open();
                            }
                        },
                        Kirigami.Action {
                            icon.name: "link"
                            text: i18n("Open")

                            onTriggered: Qt.openUrlExternally(Qt.url(html_url))
                        }
                    ]
                    contentItem: DateLabel {
                        dateTime: new Date(card.created_at)
                        prefix: "Released "
                    }

                    Rectangle {
                        id: highlight

                        property bool update: false

                        anchors.fill: parent
                        border.color: Kirigami.Theme.positiveTextColor
                        border.width: 1
                        color: "transparent"
                        // opacity: 0.5
                        radius: parent.background.radius
                        visible: VersionRegistry.downloaded(card.tag_name, root.rawRepo) || (update && !update)

                        Connections {
                            function onDownloadedChanged() {
                                highlight.update = !highlight.update;
                            }

                            target: VersionRegistry
                        }
                    }
                }
            }
        }

        Kirigami.LoadingPlaceholder {
            Layout.fillHeight: false
            Layout.fillWidth: true
            determinate: request.totalPages !== -1
            progressBar.to: request.totalPages
            progressBar.value: request.currentPage
            text: i18n("Fetching releases...")
            visible: request.running

            Behavior on progressBar.value {
                NumberAnimation {
                    duration: Kirigami.Units.longDuration
                    easing.bezierCurve: [0.25, 0.0, 0.25, 1.0, 1.0, 1.0]
                    easing.type: Easing.BezierSpline
                }
            }
        }

        Kirigami.PlaceholderMessage {
            Layout.fillHeight: true
            Layout.fillWidth: true
            // @disable-check M17
            icon.name: "network-disconnect"
            text: request.errorString
            visible: request.errorString !== ""
        }

        Kirigami.PlaceholderMessage {
            Layout.fillHeight: true
            Layout.fillWidth: true
            // @disable-check M17
            icon.name: "edit-none"
            text: i18n("No results found 😢")
            visible: resultList.model.length === 0 && !request.running
        }
    }
    // onCurrentPageChanged: progress.value = currentPage
}

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard
import org.kde.kirigamiaddons.settings as KirigamiSettings

import dev.fishies.godl

GodlConfigPage {
    id: root

    title: i18n("General")

    FormCard.FormCard {
        ConfigStringListFormCard {
            configValue: Configuration.downloadFilter
            defaultConfigValue: Configuration.defaultDownloadFilterValue
            help: i18n("Substrings to look for in remote asset names to highlight the one you most likely want. The defaults are probably okay, but if you find that some assets don't show up, add them to this list.")
            text: i18n("Add new filter")

            onConfigChanged: (i, v) => Configuration.downloadFilter[i] = v
            onConfigRemoved: i => Configuration.downloadFilter.splice(i, 1)
            onConfigSet: x => {
                Configuration.downloadFilter = x;
                configValue = Configuration.downloadFilter;
            }
        }

        ConfigStringListFormCard {
            id: sourcesConfig

            configValue: Configuration.sources
            defaultConfigValue: Configuration.defaultSourcesValue
            help: i18n("Release API sources. If the source starts with a '/', it'll be considered to be a GitHub repository, otherwise it'll be assumed to be an external source with a matching API to that of GitHub's")
            text: i18n("Add new source")

            fieldActions: [
                Component {
                    Kirigami.Action {
                        required property string data
                        required property int index

                        icon.name: index === 0 ? "favorite-favorited" : "favorite"

                        onTriggered: {
                            let list = Configuration.sources;
                            list.splice(list.indexOf(data), 1);
                            list.unshift(data);
                            Configuration.sources = list;
                            sourcesConfig.refreshItems();
                        }
                    }
                }
            ]

            onConfigChanged: (i, v) => Configuration.sources[i] = v
            onConfigRemoved: i => Configuration.sources.splice(i, 1)
            onConfigSet: x => {
                Configuration.sources = x;
                configValue = Configuration.sources;
            }
        }
    }
}

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard
import org.kde.kirigamiaddons.settings as KirigamiSettings

import org.fishy.godl

GodlConfigPage {
    id: root

    title: i18n("General")

    FormCard.FormCard {
        ConfigStringListFormCard {
            configValue: Config.downloadFilter
            defaultConfigValue: Config.defaultDownloadFilterValue
            text: i18n("Add new filter")

            onConfigChanged: (i, v) => Config.downloadFilter[i] = v
            onConfigRemoved: i => Config.downloadFilter.splice(i, 1)
            onConfigSet: x => {
                Config.downloadFilter = x;
                configValue = Config.downloadFilter;
            }
        }

        ConfigStringListFormCard {
            configValue: Config.sources
            defaultConfigValue: Config.defaultSourcesValue
            text: i18n("Add new source")

            onConfigChanged: (i, v) => Config.sources[i] = v
            onConfigRemoved: i => Config.sources.splice(i, 1)
            onConfigSet: x => {
                Config.sources = x;
                configValue = Config.sources;
            }
        }
    }
}

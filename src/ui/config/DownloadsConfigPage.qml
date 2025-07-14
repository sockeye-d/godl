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
            configValue: Configuration.downloadFilter
            defaultConfigValue: Configuration.defaultDownloadFilterValue
            text: i18n("Add new filter")

            onConfigChanged: (i, v) => Configuration.downloadFilter[i] = v
            onConfigRemoved: i => Configuration.downloadFilter.splice(i, 1)
            onConfigSet: x => {
                Configuration.downloadFilter = x;
                configValue = Configuration.downloadFilter;
            }
        }

        ConfigStringListFormCard {
            configValue: Configuration.sources
            defaultConfigValue: Configuration.defaultSourcesValue
            text: i18n("Add new source")

            onConfigChanged: (i, v) => Configuration.sources[i] = v
            onConfigRemoved: i => Configuration.sources.splice(i, 1)
            onConfigSet: x => {
                Configuration.sources = x;
                configValue = Configuration.sources;
            }
        }
    }
}

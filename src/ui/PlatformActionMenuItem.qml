import Qt.labs.platform as Platform

import dev.fishies.godl
import dev.fishies.godl.qwidgets

Platform.MenuItem {
    required property QAction action

    checkable: action.checkable
    checked: action.checked
    enabled: action.enabled
    icon.name: IconConverter.name(action.icon)
    shortcut: action.shortcut
    text: action.text

    onTriggered: action.trigger()
}

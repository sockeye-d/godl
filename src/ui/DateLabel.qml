import QtQuick
import QtQuick.Controls as Controls

import dev.fishies.godl

Controls.Label {
    required property date dateTime
    property string postfix: ""
    property string prefix: ""

    text: prefix + DateConverter.relativeFormat(dateTime) + postfix

    Controls.ToolTip {
        text: dateTime.toLocaleString()
        visible: ma.containsMouse
        x: 0
        y: height
    }

    MouseArea {
        id: ma

        anchors.fill: parent
        hoverEnabled: true
    }
}

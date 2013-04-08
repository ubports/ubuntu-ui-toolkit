import QtQuick 2.0
import Ubuntu.Components 0.1

Rectangle {
    width: units.gu(50)
    height: units.gu(50)

    Label {
        text: "Panels!"
        anchors.centerIn: parent
    }

    Panel {
        Rectangle {
            anchors.fill: parent
            color: "red"
        }

        Slider {
            anchors.centerIn: parent
            width: parent.width / 2
        }
    }
}

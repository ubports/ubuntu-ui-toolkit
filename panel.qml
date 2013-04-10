import QtQuick 2.0
import Ubuntu.Components 0.1

Rectangle {
    color: "grey"
    width: units.gu(80)
    height: units.gu(80)

    Rectangle {
        width: units.gu(50)
        height: units.gu(50)
        color: "white"
        anchors.centerIn: parent

        Label {
            text: "Panels!"
            anchors.centerIn: parent
        }

        Panel {
            align: Qt.AlignLeft

            anchors {
                left: parent.left
//                right: parent.right
                bottom: parent.bottom
                top: parent.top
            }
            width: units.gu(20)
//            height: units.gu(30)

            Rectangle {
                anchors.fill: parent
                color: "black"
                opacity: 1.0
            }

            Slider {
                anchors.centerIn: parent
                width: parent.width / 1.1
                value: 50
            }
        }
    }
}

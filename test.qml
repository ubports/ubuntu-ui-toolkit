import QtQuick 2.0
import Ubuntu.Components 0.1

MainView {
    width: units.gu(40)
    height: units.gu(40)
    Item {
        anchors.fill: parent
        Page {
            title: "hello"
            ListView {
                anchors.fill: parent
                model: 20
                delegate: Rectangle {
                    width: parent.width
                    height: units.gu(8)
                    border.width: 5
                }
            }
        }
    }
}

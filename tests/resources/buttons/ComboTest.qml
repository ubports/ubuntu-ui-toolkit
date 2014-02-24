import QtQuick 2.0
import Ubuntu.Components 0.1

MainView {
    width: units.gu(40)
    height: units.gu(71)


    Column {
        anchors.fill: parent
        spacing: units.gu(1)

        ComboButton {
            text: "Press me"
            comboList: Rectangle {
                color: "blue"
                width: parent.width
                height: parent.height
            }
        }
    }
}

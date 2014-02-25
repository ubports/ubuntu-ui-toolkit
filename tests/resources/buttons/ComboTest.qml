import QtQuick 2.0
import Ubuntu.Components 0.1

MainView {
    width: units.gu(40)
    height: units.gu(71)


    Column {
        anchors.fill: parent
        spacing: units.gu(2)

        ComboButton {
            text: "Press me"
            comboList: Rectangle {
                color: "blue"
                width: parent.width
                height: units.gu(40)
            }
        }

        ComboButton {
            id: combo2
            text: "Press me"
            expanded: true
            expandedHeight: units.gu(30)
            comboList: ListView {
                anchors.fill: combo2.comboListItem
                model: 20
                delegate: Label {
                    text: "Item #" + modelData
                }
            }
        }
    }
}

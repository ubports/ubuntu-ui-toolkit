import QtQuick 2.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1

MainView {
    width: units.gu(40)
    height: units.gu(71)


    Column {
        anchors.fill: parent
        spacing: units.gu(2)

        ComboButton {
            text: "comboList is Flickable"
            Flickable {
                id: flickable
                anchors.fill: parent
                contentHeight: rect.height

                Rectangle {
                    id: rect
                    width: parent.width
                    height: units.gu(40)
                    gradient: Gradient {
                        GradientStop {
                            position: 0.0
                            color: "red"
                        }
                        GradientStop {
                            position: 0.5
                            color: "green"
                        }
                        GradientStop {
                            position: 1.0
                            color: "blue"
                        }
                    }
                }
            }
        }

        ComboButton {
            id: combo2
            text: "comboList is ListView"
            expanded: true
            expandedHeight: units.gu(30)
            comboList: ListView {
                anchors.fill: parent
                model: 20
                delegate: Standard {
                    text: "Action #" + modelData
                }
            }
        }
    }
}

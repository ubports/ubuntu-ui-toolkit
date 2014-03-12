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
            text: "collapsedHeight differs"
            collapsedHeight: units.gu(6)
        }

        ComboButton {
            text: "content is Rectangle > expanded"
            Rectangle {
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

        ComboButton {
            text: "comboList is Flickable"
            Flickable {
                id: flickable
                objectName: "Flickable"
                anchors {
                    left: parent.left
                    right: parent.right
                    top: parent.top
                }
                height: units.gu(20)
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
            UbuntuListView {
                objectName: "ListView"
                width: parent.width
                height: combo2.comboListHeight
                model: 20
                delegate: Standard {
                    text: "Action #" + modelData
                    onClicked: {
                        combo2.text = text;
                        combo2.expanded = false
                    }
                }
            }
        }

        ComboButton {
            id: combo3
            text: "auto expandHeight"
            expandedHeight: -1
            Column {
                id: column
                anchors {
                    left: parent.left
                    right: parent.right
                }
                Rectangle {
                    width: parent.width
                    height: units.gu(6)
                    color: "red"
                }
                Rectangle {
                    width: parent.width
                    height: units.gu(6)
                    color: "green"
                }
                Rectangle {
                    width: parent.width
                    height: units.gu(6)
                    color: "blue"
                }
                Rectangle {
                    width: parent.width
                    height: units.gu(6)
                    color: "yellow"
                }
            }
        }
    }
}

import QtQuick 2.0
import Ubuntu.Components 0.1

Rectangle {
    color: "grey"
    width: units.gu(80)
    height: units.gu(80)

    Rectangle {
        width: units.gu(50)
        height: units.gu(50)
        color: "lightgrey"
        anchors.centerIn: parent

        Label {
            text: "Panels everywhere!"
            anchors.centerIn: parent
        }

        Panel {
            align: Qt.AlignLeft
            anchors {
                left: parent.left
                bottom: parent.bottom
                top: parent.top
            }
            width: units.gu(20)

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

        Panel {
            align: Qt.AlignRight
            anchors {
                right: parent.right
                verticalCenter: parent.verticalCenter
            }
            height: units.gu(20)
            width: units.gu(10)

            Rectangle {
                anchors.fill: parent
                color: "yellow"
            }
        }

        Panel {
            id: bottomLeftPanel
            align: Qt.AlignBottom
            anchors {
                bottom: parent.bottom
                left: parent.left
                right: rightPanel.left
            }
            height: toolbar.height

            Item {
                id: toolbar
                anchors {
                    left: parent.left
                    right: parent.right
                    bottom: parent.bottom
                }
                height: units.gu(8)
                ItemStyle.class: "toolbar"
                property bool active: bottomLeftPanel.active
                property bool animating: bottomLeftPanel.animating
                Label {
                    anchors.centerIn: parent
                    text: "This looks like a standard toolbar"
                }
            }
        }

        Panel {
            id: rightPanel
            align: Qt.AlignBottom
            anchors {
                bottom: parent.bottom
                right: parent.right
                top: parent.top
            }
            width: units.gu(15)

            Rectangle {
                color: "pink"
                anchors.fill: parent
            }
        }

        Panel {
            align: Qt.AlignTop
            anchors {
                top: parent.top
                right: parent.right
            }
            width: units.gu(20)
            height: units.gu(20)

            Rectangle {
                color: "red"
                anchors.fill: parent
            }
        }

        Panel {
            align: Qt.AlignTop
            anchors {
                top: parent.top
                left: parent.left
            }
            width: units.gu(20)
            height: units.gu(20)

            Rectangle {
                color: "orange"
                anchors.fill: parent
            }
        }
    }
}

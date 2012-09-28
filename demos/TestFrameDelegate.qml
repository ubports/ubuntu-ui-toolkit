import QtQuick 1.1

Rectangle {
    z: -1
    anchors.fill: parent
    radius: 8
    border.color: "brown"
    border.width: 4
    //color: control.pressed ? control.pressedColor : control.color
    color: control.styleObject.color
    smooth: true
}

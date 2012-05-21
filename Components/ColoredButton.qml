import QtQuick 1.1

AbstractButton {

    id: button
    property alias color: background.color
    property color borderColor
    property int borderWidth
    property real radius: 3.0
    property color pressColor: color
    property color hoverColor: color

    // TODO: gradient background color

    Rectangle {
        id: background
        radius: parent.radius
        width: parent.width
        height: parent.height
        border.color: parent.borderColor
        border.width: parent.borderWidth
    } // background

    states: [
        State {
            name: "pressed"
            PropertyChanges { target: background; color: pressColor }
        }
    ]

}

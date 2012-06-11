import QtQuick 1.1
import "../Components"

Rectangle {
    id: window

    width: 800
    height: 300

    property bool showText: true
    property bool showIcon: true
    property bool iconLeft: true

    Button {
        id: button
        anchors {
            top: parent.top
            horizontalCenter: parent.horizontalCenter
            margins: 40
        }

        iconSource: (showIcon) ? "call_icon.png" : ""
        text: (showText) ? "A few words of text": ""
        iconPosition: (iconLeft) ? "left" : "right"
        width: 250
        height: 80
        onClicked: print("click!")
    }

    Row {
        anchors {
            bottom: parent.bottom
            horizontalCenter: parent.horizontalCenter
            margins: 40
        }

        Button {
            text: (showText) ? "Remove text" : "Add text"
            width: 180
            onClicked: showText = !showText
        }
        Button {
            text: (showIcon) ? "Remove icon" : "Add icon"
            width: 180
            onClicked: showIcon = !showIcon
        }
        Button {
            text: (iconLeft) ? "Icon right" : "Icon left"
            width: 180
            onClicked: iconLeft = !iconLeft
            enabled: (showText && showIcon)
        }
    }
} // window

import QtQuick 1.1
import "../Components"

Rectangle {
    id: window

    width: 800
    height: 600

    property bool showText: true
    property bool showIcon: true

    Button {
        id: button
        anchors {
            top: parent.top
            horizontalCenter: parent.horizontalCenter
            margins: 40
        }

        iconSource: (showIcon) ? "call_icon.png" : ""
        text: (showText) ? "A few words of text": ""
        width: 250
        height: 80
        onClicked: print("click!")
        iconPosition: "top"
    }

    Row {
        anchors {
            bottom: parent.bottom
            horizontalCenter: parent.horizontalCenter
            margins: 40
        }

        Button {
            text: (showText) ? "Remove text" : "Add text"
            onClicked: showText = !showText
        }
        Button {
            text: (showIcon) ? "Remove icon" : "Add icon"
            onClicked: showIcon = !showIcon
        }
        Button {
            width: 100
            text: "Icon Top"
            onClicked: button.iconPosition = "top"
        }
        Button {
            width: 100
            text: "Icon Bottom"
            onClicked: button.iconPosition = "bottom"
        }
        Button {
            width: 100
            text: "Icon Left"
            onClicked: button.iconPosition = "left"
        }
        Button {
            width: 100
            text: "Icon Right"
            onClicked: button.iconPosition = "right"
        }
    }
} // window

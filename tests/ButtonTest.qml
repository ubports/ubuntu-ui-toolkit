import QtQuick 1.1
import "../Components"

Rectangle {
    id: window

    width: 800
    height: 600

    Column {
        anchors.centerIn: parent
        spacing: 5

        Button {
            width: 200
            height: 100
            text: "text only (centered)\nwith border"
            textColor: "white"
            onClicked: print("clicked text-only button")
        }
        Button {
            icon: "call_icon.png"
            height: 100
            width: 200
            iconHeight: height - 20
            onClicked: print("Clicked icon-only button")
            iconPosition: "top"
        }
        Button {
            icon: "call_icon.png"
            height: 50
            width: 200
            color: "orange"
            pressColor: "pink"
            hoverColor: "yellow"
            text: "icon on right"
            iconPosition: "right"
            iconHeight: height - 10
            borderWidth: 0
        }
        Row {
            spacing: 4
            Button {
                icon: "call_icon.png"
                height: 100
                width: 98
                iconWidth: 50
                color: "green"
                text: "Icon on top"
                iconPosition: "top"
            } // Button
            Button {
                icon: "call_icon.png"
                height: 100
                width: 98
                iconWidth: 50
                color: "red"
                text: "Text on top"
                iconPosition: "bottom"
            } // Button
        } // Row
        Button {
            icon: "call_icon.png"
            height: 50
            width: 200
            color: "orange"
            text: "icon on left"
            iconPosition: "left"
            iconHeight: height - 10
        }
    } // column
} // window

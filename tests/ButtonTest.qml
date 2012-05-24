import QtQuick 1.1
import "../Components"

Rectangle {
    id: window

    width: 800
    height: 600

    Row {
        anchors.centerIn: parent
        width: 650
        spacing: 5
        Column {
            width: 200
            spacing: 5

            Button {
                width: 200
                height: 100
                text: "text only (centered)\nwith border"
                onClicked: print("clicked text-only button")
            }
            Button {
                icon: "call_icon.png"
                height: 100
                width: 200
                onClicked: print("Clicked icon-only button")
                iconPosition: "top"
            }
            Button {
                icon: "call_icon.png"
                height: 50
                width: 200
                text: "icon on right"
                iconPosition: "right"
            }
            Row {
                spacing: 4
                Button {
                    icon: "call_icon.png"
                    height: 100
                    width: 98
                    text: "Icon on top"
                    iconPosition: "top"
                } // Button
                Button {
                    icon: "call_icon.png"
                    height: 100
                    width: 98
                    text: "Text on top"
                    iconPosition: "bottom"
                } // Button
            } // Row
            Button {
                icon: "call_icon.png"
                height: 50
                width: 200
                text: "icon on left"
                iconPosition: "left"
            }
        } // Column
        Column {
            width: 200
            Button { height: 50; width: 200; color: "pink"; hoverColor: "green"}
        } // Column
        Column {
            width: 200
            Text {
                text: "tadaa"
            }
        } // Column
    } // Row
} // window

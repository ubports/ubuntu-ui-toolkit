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
            width: 210
            spacing: 5
            Text {
                text: "text and icons"
                anchors.horizontalCenter: parent.horizontalCenter
            }
            Button {
                width: 200
                height: 80
                text: "text only (centered)\nwith border"
                onPressed: print("pressed text-only button")
            }
            Button {
                icon: "call_icon.png"
                width: 200
                height: 80
                onPressed: print("pressed icon-only button")
                iconPosition: "top"
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
                width: 200
                height: 100
                icon: "call_icon.png"
                text: "icon on right"
                iconPosition: "right"
            }
            Button {
                icon: "call_icon.png"
                height: 50
                width: 200
                text: "smaller icon on left"
                iconPosition: "left"
            }
        } // Column
        Column {
            width: 160
            spacing: 5
            Text {
                text: "color and text-size"
                anchors.horizontalCenter: parent.horizontalCenter
            }
            Button {
                icon: "call_icon.png";
                iconPosition: "right";
                color: "green";
                text: "grass color";
                textColor: "white" }
            Button {
                text: "changing colors"
                color: "pink"
                hoverColor: "yellow"
                pressColor: "purple"
            }
            Button { text: "fixed color"; color: "orange"; hoverColor: color; pressColor: color }
            Button { text: "borderless blue"; color: "blue"; textColor: "white"; borderWidth: 0.0 }
            Button { text: "thick red border"; borderColor: "red"; borderWidth: 4.0 }
            Button { text: "large red text"; textSize: "large"; textColor: "red" }
            Button { text: "larger text"; textSize: "x-large"; textColor: "white"; color: "#123" }
            Button { text: "huge text"; textSize: "xx-large"; color: "#abc" }
            Button { text: "small blue text"; textSize: "small"; textColor: "blue" }
        } // Column
        Column {
            width: 200
            Text {
                text: "tadaa"
            }
        } // Column
    } // Row
} // window

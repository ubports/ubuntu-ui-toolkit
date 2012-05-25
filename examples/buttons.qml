import QtQuick 1.1
import "../Components"

Rectangle {
    id: window

    width: 800
    height: 600

    Row {
        anchors.centerIn: parent
        width: 400
        spacing: 5
        Column {
            width: 210
            spacing: 5
            Text {
                text: "text and icons"
                anchors.horizontalCenter: parent.horizontalCenter
            }
            ColoredButton {
                width: 200
                height: 80
                text: "text only (centered)\nwith border"
                onClicked: print("clicked text-only ColoredButton")
            }
            ColoredButton {
                iconSource: "call_icon.png"
                width: 200
                height: 80
                onClicked: print("clicked icon-only ColoredButton")
                iconPosition: aboveText
            }
            Row {
                spacing: 4
                ColoredButton {
                    iconSource: "call_icon.png"
                    height: 100
                    width: 98
                    text: "Icon on top"
                    iconPosition: aboveText
                } // ColoredButton
                ColoredButton {
                    iconSource: "call_icon.png"
                    height: 100
                    width: 98
                    text: "Text on top"
                    iconPosition: belowText
                } // ColoredButton
            } // Row
            ColoredButton {
                width: 200
                height: 100
                iconSource: "call_icon.png"
                text: "icon on right"
                iconPosition: rightOfText
            }
            ColoredButton {
                iconSource: "call_icon.png"
                height: 50
                width: 200
                text: "smaller icon on left"
                iconPosition: leftOfText
            }
        } // Column
        Column {
            width: 160
            spacing: 5
            Text {
                text: "color and text-size"
                anchors.horizontalCenter: parent.horizontalCenter
            }
            ColoredButton {
                iconSource: "call_icon.png"
                iconPosition: rightOfText
                color: "green"
                text: "grass color"
                textColor: "white" }
            ColoredButton {
                text: "changing colors"
                color: "pink"
                hoverColor: "yellow"
                pressColor: "purple"
            }
            ColoredButton {
                text: "disabled"
                enabled: false
                color: "pink"
                hoverColor: "yellow"
                pressColor: "purple"
                onClicked: print("")
            }
            ColoredButton {
                text: "fixed color"
                color: "orange"
                hoverColor: color
                pressColor: color
                onClicked: print("clicked fixed color button")
            }
            ColoredButton { text: "borderless blue"; color: "blue"; textColor: "white"; borderWidth: 0.0 }
            ColoredButton { text: "thick red border"; borderColor: "red"; borderWidth: 4.0 }
            ColoredButton { text: "large red text"; textSize: "large"; textColor: "red" }
            ColoredButton { text: "larger text"; textSize: "x-large"; textColor: "white"; color: "#123" }
            ColoredButton { text: "huge text"; textSize: "xx-large"; color: "#abc" }
            ColoredButton { text: "small blue text"; textSize: "small"; textColor: "blue" }
        } // Column
    } // Row
} // window

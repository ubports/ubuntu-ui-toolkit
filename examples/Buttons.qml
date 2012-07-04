/*
 * Copyright 2012 Canonical Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 1.1
import "../Components"

Rectangle {
    id: window

    width: 600
    height: 500

    Column {
        id: buttons
        anchors {
            top: parent.top
            horizontalCenter: parent.horizontalCenter
            margins: 20
        }
        spacing: 5

        Rectangle {
            color: "#000088"
            anchors.horizontalCenter: parent.horizontalCenter
            width: parent.width
            height: 60
            radius: 5

            Row {
                anchors {
                    horizontalCenter: parent.horizontalCenter
                    verticalCenter: parent.verticalCenter
                }
                Button {
                    text: "Some"
                    color: "blue"
                }
                Button {
                    text: "small"
                    color: "purple"
                    pressedColor: "pink"
                }
                Button {
                    text: "colorful"
                    color: "#ff0000"
                }
                Button {
                    text: "buttons"
                    color: "orange"
                    pressedColor: "#ffff00"
                }
            } // Row
        } // Rectangle

        Row {
            Button {
                iconSource: "call_icon.png"
                text: "Default"
                width: 180
                height: 50
                onClicked: print("Clicked default button")
            }
            Button {
                iconSource: "call_icon.png"
                text: "Disabled"
                enabled: false
                width: 180
                height: 50
                onClicked: print("Clicked disabled button")
            }
        } // Row
        Row {
            Button {
                iconSource: "call_icon.png"
                text: "Green"
                color: "#37b301"
                width: 180
                height: 50
                onClicked: print("Clicked green button")
            }
            Button {
                iconSource: "call_icon.png"
                text: "Press me"
                pressedColor: "#dd4f22"
                width: 180
                height: 50
                onClicked: print("Clicked press-me button")
            }
        } // Row
        Row {
            Button {
                text: "Small blue text only"
                textSize: "small"
                textColor: "blue"
                width: 180
                height: 50
            }
            Button {
                text: "Larger red text"
                textSize: "x-large"
                textColor: "red"
                width: 180
                height: 50
            }
        } // Row
        Row {
            Button {
                iconSource: "call_icon.png"
                iconPosition: "right"
                text: "Icon right"
                width: 180
                height: 50
            }
            Button {
                iconSource: "call_icon.png"
                width: 180
                height: 50
            }
        } // Row

        Rectangle {
            color: "#000088"
            width: parent.width
            height: 140
            radius: 5
            anchors.margins: 50

            Column {
                anchors {
                    horizontalCenter: parent.horizontalCenter
                    verticalCenter: parent.verticalCenter
                }
                id: flexibleButtons
                spacing: 5

                property bool showText: true
                property bool showIcon: true
                property bool iconLeft: true

                Row {
                    Button {
                        iconSource: (flexibleButtons.showIcon) ? "call_icon.png" : ""
                        text: (flexibleButtons.showText) ? "Flexible.." : ""
                        iconPosition: (flexibleButtons.iconLeft) ? "left" : "right"
                        width: 170
                        height: 70
                    }
                    Button {
                        iconSource: (flexibleButtons.showIcon) ? "call_icon.png" : ""
                        text: (flexibleButtons.showText) ? "..anchors" : ""
                        iconPosition: (flexibleButtons.iconLeft) ? "left" : "right"
                        color: "#37b301"
                        width: 170
                        height: 70
                    }
                } // Row
                Row {
                    anchors. horizontalCenter: parent.horizontalCenter
                    Button {
                        text: (flexibleButtons.showText) ? "Remove text" : "Add text"
                        width: 115
                        color: "#888"
                        onClicked: flexibleButtons.showText = !flexibleButtons.showText
                        textSize: "large"
                    }
                    Button {
                        text: (flexibleButtons.showIcon) ? "Remove icon" : "Add icon"
                        width: 115
                        color: "#888"
                        onClicked: flexibleButtons.showIcon = !flexibleButtons.showIcon
                    }
                    Button {
                        text: (flexibleButtons.iconLeft) ? "Icon right" : "Icon left"
                        width: 115
                        color: "#888"
                        onClicked: flexibleButtons.iconLeft = !flexibleButtons.iconLeft
                        enabled: (flexibleButtons.showText && flexibleButtons.showIcon)
                    }
                } // Row
            } // Column
        } // Rectangle
    } // Column
} // window

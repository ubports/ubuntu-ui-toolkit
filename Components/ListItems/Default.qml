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
import ".."

/*!
    \qmlclass Default
    \inqmlmodule UbuntuUIToolkit
    \brief The Default class is DOCME

    \b{This component is under heavy development.}

*/
Item {
    id: listItem

    property alias text: label.text
    property alias iconSource: icon.source
    property Item control
    property bool progression: false

    property alias subText: subtitle.text

    property bool selected: false

    property bool topSeparator: false
    property bool bottomSeparator: true

    property bool __testing: false

    property int __padding: 10
    //property bool __hasSubLabel: subtext != ""
    property color __textColor: selected ? "#f37505" : Qt.rgba(0.4, 0.4, 0.4, 1.0)

    property bool __split: (listItem.control && (listItem.progression == true)) ? true : false


    height: 54
    width: 250

    signal clicked
    //onClicked: selected = !selected

    AbstractButton {
        anchors.fill: parent
        enabled: listItem.enabled
        // NOTE: If there is a control, and progression == false,
        // then the control must accept the clicked() signal.
        // TODO: Check whether clicked() is there and don't call it if not.
        onClicked: (listItem.control && (listItem.progression == false)) ? listItem.control.clicked() : listItem.clicked()
    }

    Item {
        id: visuals

        anchors.fill: parent

        Image {
            id: topSeparatorLine

            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            height: visible ? 2 : 0
            //source: "artwork/ListItemSeparator.png"
            source: "artwork/divider_Vertical.png"
            visible: listItem.topSeparator
        }

        Image {
            id: bottomSeparatorLine

            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            height: visible ? 2 : 0
            //source: "artwork/ListItemSeparator.png"
            source: "artwork/divider_Vertical.png"
            visible: listItem.bottomSeparator
        }

        Image {
            visible: listItem.selected
            anchors.top: visuals.top
            anchors.topMargin: 2
            anchors.right: visuals.right
            anchors.rightMargin: 2
            source: "artwork/list_item_selected_triangle.png"
        }


        Item {
            id: leftVisuals

            //property bool hasIcon: listItem.iconSource != ""
            visible: listItem.iconSource != ""

            width: visible ? icon.width + 10 : 0
            Rectangle {
                // for testing only
                visible: listItem.__testing
                anchors.fill: parent;
                color:"green"
            }

            anchors {
                left: parent.left
                top: parent.top
                bottom: parent.bottom
            }

            Image {
                id: icon
                visible: parent.visible
                width: visible ? 36 : 0
                opacity: listItem.enabled ? 1.0 : 0.5
                fillMode: Image.PreserveAspectFit
                anchors {
                    verticalCenter: parent.verticalCenter
                    left: parent.left
                    leftMargin: 5
                }
            } // Image

            BorderImage {
                id: iconFrame

                //visible: !listItem.isIcon
                visible: parent.visible
                source: "artwork/ListItemFrame.png"
                anchors.fill: icon
                anchors.bottomMargin: -1
                border.left: 3
                border.right: 3
                border.top: 3
                border.bottom: 3
                horizontalTileMode: BorderImage.Stretch
                verticalTileMode: BorderImage.Stretch
            }
        }

        Item { // encapsulation
            id: middleVisuals

            property bool hasSubText: listItem.subText != ""

            height: hasSubText ? label.height + subtitle.height : label.height
            anchors {
                left: leftVisuals.right
                right: rightVisuals.left
                verticalCenter: parent.verticalCenter
            }

            Rectangle {
                color: "white"
                anchors.fill: parent
                visible: listItem.__testing
            }

            TextCustom {
                id: label
                anchors {
                    //verticalCenter: parent.verticalCenter
                    top: parent.top
                    left: parent.left
                    leftMargin: 5
                }

                elide: Text.ElideRight

                color: listItem.__textColor
                style: Text.Raised
                styleColor: "white"
                opacity: listItem.enabled ? 1.0 : 0.5

                Rectangle {
                    // for testing only
                    z: -1
                    anchors.fill: parent
                    color: "yellow"
                    visible: listItem.__testing
                }
            } // TextCustom

            TextCustom {
                id: subtitle
                anchors {
                    bottom: parent.bottom
                    left: parent.left
                    leftMargin: 5
                }
                width: parent.width
                elide: Text.ElideRight
            }
        }

        Item {
            id: rightVisuals
            anchors {
                top: parent.top
                bottom: parent.bottom
                right: parent.right
            }
            //width: childrenRect.width

            width: {
                var w = 0
                if (listItem.control) w += listItem.control.width;
                if (listItem.progression) w += progressItem.width;
                if (listItem.__split) w += progressionDivider.width;
                return w;
            }

            Rectangle {
                anchors.fill: parent
                visible: listItem.__testing
                color: "blue"
                width: 200
            }

            property alias progressItem: progressItem

            Item {
                id: progressItem
                height: parent.height
                width: height // square
                anchors {
                    right: parent.right
                    top: parent.top
                    bottom: parent.bottom
                }

                Image {
                    id: progressIcon
                    source: "artwork/arrow_Progression.png"
                    //                text: ">"
                    //                fontSize: "xx-large"
                    anchors.centerIn: parent
                    visible: listItem.progression
                } // TextCustom

                Image {
                    id: progressionDivider
                    visible: listItem.__split
                    anchors {
                        top: parent.top
                        bottom: parent.bottom
                        bottomMargin: bottomSeparatorLine.height
                        topMargin: topSeparatorLine.height
                    }
                    width: 1
                    source: "artwork/divider_Vertical.png"
                }
            }
            property Item previousControl: null
            function controlChanged() {
                if (rightVisuals.previousControl) rightVisuals.previousControl.visible = false;
                if (listItem.control) {
                    listItem.control.parent = rightVisuals;
                    listItem.control.anchors.verticalCenter = rightVisuals.verticalCenter
                    listItem.control.anchors.right = (listItem.progression) ? rightVisuals.progressItem.left : rightVisuals.right;
                }
                rightVisuals.previousControl = listItem.control;
            }

            Connections {
                target: listItem
                onControlChanged: controlChanged()
            }

            Component.onCompleted: controlChanged()
        }

        //        Rectangle {
        //            id: background
        //            z: -1
        //            color: "#eeeeee"
        //            anchors.fill: parent
        //            border.width: 2
        //            border.color: "black"
        //            radius: 5
        //        }

    } // visuals
}

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
import Qt.labs.shaders 1.0

/*!
    \qmlclass ListItem
    \inqmlmodule UbuntuUIToolkit
    \brief The ListItem class is DOCME

    \b{This component is under heavy development.}

*/
Item {
    id: listItem

    property alias text: label.text
    property alias iconSource: icon.source
    property Item control
    property bool progression: false

    height: 40 //middleVisuals.height
    width: 380 //(parent) ? parent.width : 0

    signal clicked

    AbstractButton {
        anchors.fill: parent
        // NOTE: If there is a control, and progression == false,
        // then the control must accept the clicked() signal.
        // TODO: Check whether clicked() is there and don't call it if not.
        onClicked: (listItem.control && (listItem.progression == false)) ? listItem.control.clicked() : listItem.clicked()
    }

    Item {
        id: leftVisuals

        property bool hasIcon: listItem.iconSource != ""
        width: hasIcon ? middleVisuals.height : 0
        visible: hasIcon

        anchors {
            left: parent.left
            top: parent.top
            bottom: parent.bottom
        }

        Image {
            id: icon
            visible: iconSource ? true : false
            fillMode: Image.PreserveAspectFit
            anchors {
                centerIn: parent
            }
            width: parent.width - 20
        } // Image
    }

    Item { // encapsulation
        id: middleVisuals
        height: 40
        anchors {
            left: leftVisuals.right
            right: rightVisuals.left
            //top: listItem.top
            verticalCenter: parent.verticalCenter
        }

        TextCustom {
            id: label
            anchors {
                verticalCenter: parent.verticalCenter
                left: parent.left
            }

            Rectangle {
                // for testing only
                z: -1
                anchors.fill: parent
                color: "yellow"
            }
        } // TextCustom
}

    Item {
        id: rightVisuals
        anchors {
            top: parent.top
            bottom: parent.bottom
            right: parent.right
        }

        property alias progressItem: progressItem

        TextCustom {
            id: progressItem
            text: ">"
            fontSize: "xx-large"
            anchors {
                right: parent.right
                verticalCenter: parent.verticalCenter
            }
            visible: listItem.progression
        } // TextCustom

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

    Rectangle {
        id: background
        z: -1
        color: "#eeeeee"
        anchors.fill: parent
        border.width: 2
        border.color: "black"
        radius: 5
    }
}

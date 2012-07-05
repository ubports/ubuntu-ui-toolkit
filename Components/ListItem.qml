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

    height: 60
    width: (parent) ? parent.width : 0

    signal clicked

    AbstractButton {
        anchors.fill: parent
        // NOTE: If there is a control, and progression == false,
        // then the control must accept the clicked() signal.
        // TODO: Check whether clicked() is there and don't call it if not.
        onClicked: (listItem.control && (listItem.progression == false)) ? listItem.control.clicked() : listItem.clicked()
    }

    Item { // encapsulation
        id: visuals
        anchors.fill: parent

        TextCustom {
            id: label
            anchors {
                margins: 10
                verticalCenter: parent.verticalCenter
                left: iconSource ? icon.right : parent.left
            }
        } // TextCustom

        Image {
            id: icon
            visible: iconSource ? true : false
            fillMode: Image.PreserveAspectFit
            anchors {
                // margins: 10
                verticalCenter: parent.verticalCenter
            }
            //sourceSize.width: width
            //sourceSize.height: height
            height: parent.height - 20
            //x: 10
        } // Image

        property alias progressItem: progressItem

        TextCustom {
            id: progressItem
            text: ">"
            fontSize: "xx-large"
            anchors {
                right: parent.right
                verticalCenter: visuals.verticalCenter
            }
            visible: listItem.progression
        } // TextCustom

        Rectangle {
            id: background
            z: -1
            color: "#eeeeee"
            anchors.fill: parent
            border.width: 2
            border.color: "black"
            radius: 5
        }

        property Item previousControl: null
        function controlChanged() {
            if (visuals.previousControl) visuals.previousControl.visible = false;
            print("control changed")
            if (listItem.control) {
                listItem.control.parent = visuals;
                listItem.control.anchors.verticalCenter = visuals.verticalCenter;
                listItem.control.anchors.right = (listItem.progression) ? visuals.progressItem.left : visuals.right;
            }
            visuals.previousControl = listItem.control;
        }

        Connections {
            target: listItem
            onControlChanged: controlChanged()
        }

        Component.onCompleted: controlChanged()
    } // visuals
}

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
    \qmlclass DefaultListItem
    \inqmlmodule UbuntuUIToolkit
    \brief The DefaultListItem class is DOCME

    \b{This component is under heavy development.}

*/
ListItem {
    id: listItem

    property alias text: label.text
    property alias iconSource: icon.source
    property Item control
    property bool progression: false
    signal clicked // when progression symbol is clicked.

    anchors.margins: 10

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

    AbstractButton {
        id: progressButton
        visible: listItem.progression
        width: 50
        height: 50
        anchors {
            right: parent.right
        }
        onClicked: listItem.clicked()

        TextCustom {
            anchors.centerIn: progressButton
            text: ">"
            fontSize: "xx-large"
            color: progressButton.state != "pressed" ? "black" : "brown"
        } // TextCustom
    } // progressButton

    Component.onCompleted: {
        if (control) {
            control.parent = listItem;
            control.anchors.verticalCenter = listItem.verticalCenter;
            control.anchors.right = listItem.right;
            control.z = 1;
        }
    }
}

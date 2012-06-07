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

/*!
    \qmlclass ButtonBackground
    \inqmlmodule UbuntuUIToolkit
    \brief Provides a button with a background using a Rectangle.

    The Rectangle is configurable and changes color when the user
    hovers or presses the button, or when the button is disabled.
*/
Item {
    id: bg
    z: -1

    property Item button: parent ? parent : null
    anchors.fill: button

    /*!
       \preliminary
       DOCME
    */
    property color color: "#8888cc"

    /*!
       \preliminary
       DOCME
    */
    property color borderColor: "black"

    /*!
      \preliminary
      DOCME
    */
    property int borderWidth: 2.0

    /*!
       \preliminary
       DOCME
    */
    property real radius: 3.0

    /*!
       \preliminary
       DOCME
    */
    property color pressColor: Qt.darker(bg.color, 1.2)

    /*!
       \preliminary
       DOCME
    */
    property color hoverColor: Qt.lighter(bg.color, 1.25)

    /*!
      \preliminary
      DOCME
    */
    property color disabledColor: "#888888"

    //property alias background: rectbg

    Rectangle {
        id: rectbg
        radius: parent.radius
        width: parent.width
        height: parent.height
        color: parent.color
        border.color: parent.borderColor
        border.width: parent.borderWidth
    } // background

    states: [
        State {
            name: "idle"
            PropertyChanges { target: rectbg; color: bg.color }
        },
        State {
            name: "pressed"
            PropertyChanges { target: rectbg; color: bg.pressColor }
        },
        State {
            name: "hovered"
            PropertyChanges { target: rectbg; color: bg.hoverColor }
        },
        State {
            name: "disabled"
            PropertyChanges { target: rectbg; color: bg.disabledColor }
        }
    ]

    state: button ? button.state : "undefined"
}

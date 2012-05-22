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
    \qmlclass ColoredButton
    \inqmlmodule UbuntuUIToolkit
    \brief The ColoredButton class is DOCME

    \b{This component is under heavy development.}

    The ColoredButton class is part of the \l{UbuntuUIToolkit} module.
    It adds a colored background, border, and changes of background color
    depending on the state, to the AbstractButton.
*/
AbstractButton {
    id: button

    /*!
       \preliminary
       DOCME
    */
    property color color: "blue"

    /*!
       \preliminary
       DOCME
    */
    property color borderColor

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
    property color pressColor: color

    /*!
       \preliminary
       DOCME
    */
    property color hoverColor: color

    Rectangle {
        id: background
        radius: parent.radius
        width: parent.width
        height: parent.height
        color: button.color
        border.color: parent.borderColor
        border.width: parent.borderWidth
    } // background

    states: [
        State {
            name: "default"
            PropertyChanges { target: background; color: button.color }
        },
        State {
            name: "pressed"
            PropertyChanges { target: background; color: button.pressColor }
        },
        State {
            name: "hovered"
            PropertyChanges { target: background; color: button.hoverColor }
        }
    ]
}

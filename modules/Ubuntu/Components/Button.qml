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

import QtQuick 2.0
import "." 0.1 as Theming

/*!
    \qmltype Button
    \inqmlmodule Ubuntu.Components 0.1
    \ingroup ubuntu
    \brief The Button class is DOCME

    \b{This component is under heavy development.}

    Examples:
    \qml
        Column {
            Button {
                text: "Send"
                onClicked: print("clicked text-only Button")
            }
            Button {
                iconSource: "icon.png"
                color: "green"
                onClicked: print("clicked icon-only Button")
            }
            Button {
                iconSource: "icon.png"
                text: "Icon on left"
                iconPosition: "left"
                onClicked: print("clicked text and icon Button")
            }
        }
    \endqml
*/
ButtonWithForeground {
    id: button

    width: 87
    height: 39
    Theming.ItemStyle.class: "Button"

    /*!
       \preliminary
       The foreground color of the button in idle state.
    */
    property color color: (Theming.ItemStyle.style) ? Theming.ItemStyle.style.color : "#e3e5e8"

    /*!
       \preliminary
       The color when the button is pressed.
    */
    property color pressedColor: color

    /*!
       \preliminary
       Specify whether the layer behind the button has a dark color.
       This will change the visuals/shading of the button.
    */
    property bool darkBorder: false
}

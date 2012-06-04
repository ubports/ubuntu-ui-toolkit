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
    \brief The ColoredButton class adds a colored background to the Button.

    \b{This component is under heavy development.}

    It adds a colored background, border, and changes of background color
    depending on the state, to the Button.

    Examples:
    \qml
        Column {
            width: 155
            spacing: 5

            ColoredButton {
                text: "text only (centered)\nwith border"
                onClicked: print("clicked text-only ColoredButton")
            }
            ColoredButton {
                iconSource: "call_icon.png"
                onClicked: print("clicked icon-only ColoredButton")
                iconPosition: "top"
            }
            ColoredButton {
                iconSource: "call_icon.png"
                text: "Icon on right"
                iconPosition: "right"
                onClicked: print("clicked on ColoredButton with text and icon")
            }
        }
    \endqml
*/

Button {
    id: button

    /*!
      \preliminary
      The properties of the background of the button
    */
    property alias color: background.color
    property alias borderColor: background.borderColor
    property alias borderWidth: background.borderWidth
    property alias radius: background.radius
    property alias pressColor: background.pressColor
    property alias hoverColor: background.hoverColor
    property alias disabledColor: background.disabledColor

    //property ButtonBackground background: ButtonBackground  { } // this doesn't work.

    ButtonBackground {
        id: background
    }
}

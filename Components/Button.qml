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
    \qmlclass Button
    \inqmlmodule UbuntuUIToolkit
    \brief The Button class adds a colored background to the Button.

    \b{This component is under heavy development.}

    It adds a colored background, border, and changes of background color
    depending on the state, to the Button.

    Examples:
    \qml
        Column {
            width: 155
            spacing: 5

            Button {
                text: "text only (centered)\nwith border"
                onClicked: print("clicked text-only Button")
            }
            Button {
                iconSource: "call_icon.png"
                onClicked: print("clicked icon-only Button")
                iconPosition: "top"
                color: "green"
            }
            Button {
                iconSource: "call_icon.png"
                text: "Icon on right"
                iconPosition: "right"
                onClicked: print("clicked on Button with text and icon")
            }
        }
    \endqml
*/

ButtonFace {
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

    // This doesn't work, so instead we define the aliases above.
    // We do the same in TabButton. See also:
    // https://bugreports.qt-project.org/browse/QTBUG-20200
    // https://bugreports.qt-project.org/browse/QTBUG-15269
    // It may be fixed in Qt5.
    //property ButtonBackground background: ButtonBackground { }

    ButtonBackground {
        id: background
    }
}

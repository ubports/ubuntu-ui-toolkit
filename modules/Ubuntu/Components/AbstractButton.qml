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
    \qmlabstract AbstractButton
    \inqmlmodule Ubuntu.Components 0.1
    \brief The AbstractButton class defines the behavior of the button.

    \b{This component is under heavy development.}

    This class defines the behavior of the button: it defines the MouseArea
    and the states.
*/
Item {
    id: button

    /*!
       \preliminary
       This handler is called when there is a mouse click on the button
       and the button is not disabled.
    */
    signal clicked

    Keys.onEnterPressed: clicked()
    Keys.onReturnPressed: clicked()

    /*!
      \preliminary
      The mouse area of the abstract button.
      May be assigned a different mouse area if, for example, the area where
      the control is shown is not the same as the area where it accepts mouse
      events. This is used in list items with controls.
     */
    property MouseArea mouseArea: defaultMouseArea

    /*!
     \preliminary
      True if the user presses a mouse button in the button's mouse area.
     */
    property bool pressed

    /*!
      \preliminary
      True if the mouse cursor hovers over the button's mouse area.
     */
    property bool hovered

    MouseArea {
        id: defaultMouseArea
        anchors.fill: parent
    }

    onMouseAreaChanged: hiddenFunctions.updateMouseArea()

    Item {
        id: hiddenFunctions

        function updateMouseArea() {
            if (button.mouseArea) {
                button.mouseArea.clicked.connect(button.clicked);
                button.mouseArea.pressedChanged.connect(hiddenFunctions.mouseAreaPressed);
                button.mouseArea.entered.connect(hiddenFunctions.mouseAreaEntered);
                button.mouseArea.exited.connect(hiddenFunctions.mouseAreaEntered);
            }
        }

        function mouseAreaPressed() {
            button.pressed = mouseArea.pressed;
        }

        function mouseAreaEntered() {
            button.hovered = true;
        }

        function mouseAreaExited() {
            button.hovered = false;
        }
    }
}

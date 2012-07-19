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
    \inqmlmodule UbuntuUIToolkit
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

    MouseArea {
        id: mouse_area

        enabled: parent.enabled
        hoverEnabled: parent.enabled
        anchors.fill: parent

        // the check for button.enabled is not really needed since if
        // it is false, mouse_area.enabled is also false.
        // But this makes the behavior more clear.
        onClicked: if (button.enabled) parent.clicked()
    }

    /*!
      \internal
     */
    function __getState() {
        if (!button.enabled)
            return "disabled"
        else if (mouse_area.pressed)
            return "pressed"
        else if (mouse_area.containsMouse)
            return "hovered"
        else
            return "idle"
    }

    state: {
        return __getState();
    }
}

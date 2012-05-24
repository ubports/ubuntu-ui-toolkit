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
    \qmlclass AbstractButton
    \inqmlmodule UbuntuUIToolkit
    \brief The AbstractButton class is DOCME

    \b{This component is under heavy development.}

    The AbstractButton class is part of the \l{UbuntuUIToolkit} module.
    This class defines the behavior of the button: it defines the MouseArea
    and the states.
*/
Item {
    /*!
       \preliminary
       DOCME
    */
    property bool enabled: true

    /*!
       \preliminary
       Use to manually set the "pressed" state of the button. This is not
       necessary in the normal use case, but is useful when a child item eats
       the mouse events (e.g. a DragArea).
       This is a clumsy workaround for the lack of a MouseProxy element
       (see http://bugreports.qt.nokia.com/browse/QTBUG-13007).
    */
    //property bool pressed: false

    /*!
       \preliminary
       DOCME
    */
    signal clicked

    MouseArea {
        id: mouse_area

        enabled: parent.enabled
        hoverEnabled: parent.enabled
        anchors.fill: parent
        onClicked: parent.clicked()
    }

    state: {
        if (mouse_area.pressed)
            return "pressed"
        else if (mouse_area.containsMouse)
            return "hovered"
        else
            return "idle"
    }
}

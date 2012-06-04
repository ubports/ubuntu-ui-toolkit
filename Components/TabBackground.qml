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
    \qmlclass TabBackground
    \inqmlmodule UbuntuUIToolkit
    \brief The TabBackground extends the ButtonBackground
        with an additional "selected" state and associated
        selectedColor, which will be the color of the
        TabButton when it's associated tab is selected.
*/
ButtonBackground {
    id: tabbutton

    /*!
      \preliminary
      The color of the button when it is selected.
     */
    property color selectedColor: "white"

    states: [
        State {
            name: "selected"
            PropertyChanges { target: background; color: tabbutton.selectedColor }
        }
    ]
}

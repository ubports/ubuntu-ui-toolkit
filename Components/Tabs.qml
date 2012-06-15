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
    \qmlclass Tabs
    \inqmlmodule UbuntuUIToolkit
    \brief TODO

    \b{This component is under heavy development.}
*/

Item {
    id: tabs

    // The row to add TabButtons to.
    property Row buttonRow: buttonRow
    property TabGroup tabGroup: tabGroup

    Row {
        id: buttonRow
        height: 40
        anchors {
            top: tabs.top
            horizontalCenter: tabs.horizontalCenter
        }
    }

    TabGroup {
        id: tabGroup
        anchors {
            top: buttonRow.bottom
            left: tabs.left
            right: tabs.right
            bottom: tabs.bottom
        }
    }
    // TODO: automatically select the first tab.
}

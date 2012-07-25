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
    \qmlclass Divider
    \inqmlmodule UbuntuUIToolkit 0.1
    \brief Divider to break up list items into groups.

    Examples: see \l Header
    \b{This component is under heavy development.}
*/
Base {
    id: listItemDivider
    height: image.implicitHeight

    __isDivider: true

    Image {
        id: image
        source: "artwork/ListItemDivider6px.png"
        anchors {
            left: parent.left
            right: parent.right
        }
    }
}

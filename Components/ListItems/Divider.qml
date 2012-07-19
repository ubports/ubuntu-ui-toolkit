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
import ".."

/*!
    \qmlclass Divider
    \inqmlmodule UbuntuUIToolkit 0.1
    \brief Header for grouping list items together

    Examples: see \l Header
    \b{This component is under heavy development.}
*/
Base {
    id: listItemDivider
    height: 6

    __isDivider: true

    Image {
        anchors.fill: parent
        source: "artwork/divider_6px.png"
        height: 6
        width: parent.width
    }
}

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

import QtQuick 2.4
import Ubuntu.Components 1.2

/*!
    \qmltype Divider
    \inqmlmodule Ubuntu.Components.ListItems 1.0
    \ingroup ubuntu-listitems
    \brief Divider to break up list items into groups.
    \note \b{The component is deprecated.}

    Examples: see \l Header
*/
Image {
    source: Qt.resolvedUrl("../../artwork/ListItemDivider6px.png")
    anchors {
        left: parent ? parent.left : undefined
        right: parent ? parent.right : undefined
    }
    height: units.gu(1)
}

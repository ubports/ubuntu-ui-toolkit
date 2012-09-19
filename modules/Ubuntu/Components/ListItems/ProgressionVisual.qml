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

// Internal helper class for the visuals of
// the progression symbol.
Item {
    width: visible ? 48 : 0

    property bool showSplit: false

    Image {
        source: "artwork/ListItemProgressionArrow.png"
        anchors.centerIn: parent
        opacity: enabled ? 1.0 : 0.5
    }

    Image {
        visible: parent.showSplit
        anchors {
            top: parent.top
            bottom: parent.bottom
        }
        width: 1
        source: "artwork/ListItemDividerVertical.png"
        opacity: enabled ? 1.0 : 0.5
    }
}

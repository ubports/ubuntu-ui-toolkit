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

import QtQuick 2.0

// internal class, to be used inside PopupBase subclasses.
Item {
    id: foreground

    property alias color: rectangle.color

    // TODO: move to themes of individual popup types
    Rectangle {
        id: rectangle
        anchors.fill: parent
        radius: units.gu(1)
    }

    // Avoid mouse events being sent to any MouseAreas that are behind the popover
    MouseArea {
        anchors.fill: parent
    }
}

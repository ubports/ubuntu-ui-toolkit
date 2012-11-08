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
//import "../mathUtils.js" as MathUtils

Item {
    // TODO: check that popover has a parent
    // TODO: get rid of this class?
    id: foreground

    property alias base: foreground.parent

    property Item caller: base.caller

    // TODO: move to theme
    // TODO: move to individual popup types.
    Rectangle {
        anchors.fill: parent
        color: "silver"
        opacity: 0.9
        radius: units.gu(2)
    }

    // Avoid mouse events being sent to any MouseAreas that are behind the popover
    MouseArea {
        anchors.fill: parent
    }
}

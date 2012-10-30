/*
 * Copyright (C) 2011-2012 Canonical Ltd
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors:
 * Lucas Beeler <lucas@yorba.org>
 */

import QtQuick 2.0

Item {
    Rectangle {
        anchors.fill: parent
        color: "yellow"
        opacity: 0.5
    }

//    color: "yellow"
//    opacity: 0.5
    id: popover

    width: units.gu(40)
    height: Math.min(childrenRect.height, maxHeight)

    // TODO: what to choose when there is no overlay?
    property real maxHeight: overlay ? 3*overlay.height/4 : units.gu(100)

    property Item overlay: null
    property Item caller

    onCallerChanged: {
        popover.x = caller.x - units.gu(5);
        popover.y = caller.y + units.gu(5);
    }
}

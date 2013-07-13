/*
 * Copyright 2013 Canonical Ltd.
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
import Ubuntu.Components 0.1

Item {
    id: pointer

    // up, down, left or right, or none to hide the pointer
    property string direction

    Loader {
        visible: (direction !== "none")

        x: (direction === "up" || direction === "down") ? -width/2.0 :
           (direction === "right") ? -width : 0
        y: (direction === "left" || direction === "right") ? -height/2.0 :
           (direction === "down") ? -height : 0
        property Item styledItem: pointer
        sourceComponent: Theme.createStyleComponent("PointerStyle.qml", pointer)
    }
}

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
import Ubuntu.Components 0.1

UbuntuShape {
    id: shape
    // Cannot use contentItem property as the overlaid controls will also be
    // re-parented to it, and those shouldn't be. Need to come up with a better
    // reparenting filter, till then we have to manipulate the z-order to have the
    // frame/background properly layouted
    z: -1

    property bool error: item.errorHighlight && !item.acceptableInput
    anchors.fill: parent
    radius: StyleUtils.itemStyleProperty("radius", "small")
    color: (error) ?
               StyleUtils.itemStyleProperty("errorFillColor", "red") :
               StyleUtils.itemStyleProperty("normalFillColor", "white")
    opacity: StyleUtils.itemStyleProperty("opacity", 1.0)
}

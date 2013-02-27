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

Image {
    id: container
    anchors.fill: parent
    source: StyleUtils.itemStyleProperty("source", "")
    smooth: true
    visible: item.running
    fillMode: Image.Stretch
    NumberAnimation on rotation {
        running: item.running
        from: 0
        to: 360
        loops: Animation.Infinite
        duration: StyleUtils.itemStyleProperty("rotationDuration", 0)
    }
}

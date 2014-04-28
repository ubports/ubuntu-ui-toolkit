/*
 * Copyright 2014 Canonical Ltd.
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
import Ubuntu.Components 1.1

Flickable {
    width: units.gu(40)
    height: units.gu(40)
    clip: true

    objectName: "FilterOwner"
    Mouse.priority: Mouse.BeforeItem

    contentWidth: content.width
    contentHeight: content.height
    Rectangle {
        id: content
        width: units.gu(100)
        height: units.gu(100)
        color: "green"
    }
}

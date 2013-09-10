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

Rectangle {
    anchors.fill: parent
    radius: width / 2
    color: UbuntuColors.darkAubergine
    antialiasing: true

    // center item
    // FIXME: Replace this with UbuntuShape when the widget has landed in the SDK.
    Rectangle {
        parent: styledItem.centerDisk
        anchors.fill: parent
        radius: width / 2;
        color: UbuntuColors.midAubergine
        antialiasing: true;
    }
}

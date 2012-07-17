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
import ".."

Item {
    id: progressionVisual
    height: parent.height
    width: visible ? 54 : 0
    visible: false

    property alias showSplit: progressionDivider.visible

    Image {
        id: progressIcon
        source: "artwork/arrow_Progression.png"
        anchors.centerIn: parent
        visible: parent.visible
        opacity: enabled ? 1.0 : 0.5
    }

    Image {
        id: progressionDivider
        visible: false
        anchors {
            top: parent.top
            bottom: parent.bottom

        }
        width: 1
        source: "artwork/divider_Vertical.png"
    }
}

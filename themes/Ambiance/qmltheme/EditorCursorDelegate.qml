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

Rectangle {
    id: cursor

    property bool topBall: item.selectionMode && item.selectionStartCursor

    color: itemStyle.color
    opacity: itemStyle.opacity
    width: itemStyle.width
    height: item.height

    onXChanged: if (item.selectionMode) item.updateCursorPosition(x, y + height / 2)
    onYChanged: if (item.selectionMode) item.updateCursorPosition(x, y + height / 2)

    Rectangle {
        id: leftPin
        width: units.gu(1.2)
        height: units.gu(1.2)
        radius: units.gu(1.2)
        smooth: true
        color: "blue"
        anchors {
            horizontalCenter: cursor.horizontalCenter
            bottom: topBall ? cursor.top : undefined
            top: !topBall ? cursor.bottom : undefined
        }

        visible: item.selectionMode
        MouseArea {
            anchors.fill: parent
            anchors.margins: -units.dp(3)
            drag {
                target: cursor
                axis: Drag.XandYAxis
            }
        }
    }

    Timer {
        interval: itemStyle.blinkTimeoutShown
        running: item.showCursor && (itemStyle.blinkTimeoutShown > 0) && (itemStyle.blinkTimeoutHidden > 0) && !item.selectionMode
        repeat: true
        onTriggered: {
            interval = (interval == itemStyle.blinkTimeoutShown) ?
                        itemStyle.blinkTimeoutHidden : itemStyle.blinkTimeoutShown;
            item.timerShowCursor = !item.timerShowCursor;
        }
    }
}

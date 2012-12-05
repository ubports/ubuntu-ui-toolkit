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

    property bool showCursor: item.hasOwnProperty("showCursor") ? item.showCursor : item.editor.cursorVisible
    property bool timerShowCursor: true

    visible: showCursor && timerShowCursor
    color: itemStyle.color
    opacity: itemStyle.opacity
    width: itemStyle.width
    height: item.height

    Timer {
        interval: itemStyle.blinkTimeoutShown
        running: showCursor && (itemStyle.blinkTimeoutShown > 0) && (itemStyle.blinkTimeoutHidden > 0) && itemStyle.blinking
        repeat: true
        onTriggered: {
            interval = (interval == itemStyle.blinkTimeoutShown) ?
                        itemStyle.blinkTimeoutHidden : itemStyle.blinkTimeoutShown;
            timerShowCursor = !timerShowCursor;
        }
    }
}

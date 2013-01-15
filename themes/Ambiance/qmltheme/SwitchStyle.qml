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

QtObject {
    property url crossSource: Qt.resolvedUrl("artwork/Cross.png")
    property url checkMarkSource: Qt.resolvedUrl("artwork/CheckMark.png")
    property real selectedImageOpacity: 1.0
    property real unselectedImageOpacity: 0.4
    property real thumbSpacing
    property color checkedThumbColor: "#DD4814"
    property color uncheckedThumbColor: "#bbbbbb"
    property color backgroundColor: Qt.rgba(0.78, 0.78, 0.78, 0.1) //"#c7c7c71a" // ubuntu-orange with 0.1 opacity
    property real thumbWidth: units.gu(4)
    property real thumbHeight: units.gu(4)
}

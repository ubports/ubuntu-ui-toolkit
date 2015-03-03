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
import Ubuntu.Components 1.1

Item {
    property int intValue: 100
    property bool boolValue: false
    property real realValue: 123.45
    property double doubleValue: 32155.2355
    property string string: "component string"
    property url url: Qt.resolvedUrl("SaveSupportedTypes.qml")

    property date date: new Date()
    property point point: Qt.point(10, 10)
    property rect rect: Qt.rect(0, 0, 20, 20)
    property size size: Qt.size(20, 30)
    property color color: "red"
    property font font: Qt.font({family: "Courrier New"})
    property vector2d vector2d: Qt.vector2d(1, 2)
    property vector3d vector3d: Qt.vector3d(1, 2, 3)
    property vector4d vector4d: Qt.vector4d(1, 2, 3, 4)
    property quaternion quaternion: Qt.quaternion(0.5, 1, 2, 3)
    property matrix4x4 matrix4x4: Qt.matrix4x4(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16)
    id: testItem
    objectName: "testItem"
    StateSaver.properties: "intValue, boolValue, realValue, doubleValue, string, url,"+
                           "date, point, rect, size, time, color, font, vector2d, vector3d, vector4d, quaternion, matrix4x4"
}

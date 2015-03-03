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

//![property-group-saver]
import QtQuick 2.0
import Ubuntu.Components 1.1

Rectangle {
    id: root
    width: units.gu(40)
    height: units.gu(71)
    color: "green"
    StateSaver.properties: "border.color, border.width"

    //![alt-group-saver]
    property Rectangle innerRect: Rectangle {
        id: inner
        width: units.gu(20)
        height: units.gu(40)
        parent: root
        z: 0
        anchors.centerIn: root
        StateSaver.properties: "color"
    }
    //![alt-group-saver]

    Button {
        text: "Change"
        anchors.centerIn: parent
        z: 1
        onClicked: {
            root.color = Qt.rgba(Math.random(1), Math.random(1), Math.random(1), 1);
            root.border.width = units.gu(2) * Math.random(1)
            root.border.color = Qt.rgba(Math.random(1), Math.random(1), Math.random(1), 1);
            innerRect.color = Qt.rgba(Math.random(1), Math.random(1), Math.random(1), 1);
        }
    }
}
//![property-group-saver]

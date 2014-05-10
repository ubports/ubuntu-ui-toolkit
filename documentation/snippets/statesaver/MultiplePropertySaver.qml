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

//![group-saver]
import QtQuick 2.0
import Ubuntu.Components 1.1

Rectangle {
    id: root
    width: units.gu(40)
    height: units.gu(71)
    color: "green"
    StateSaver.properties: "color, border.color, border.width"

    Button {
        text: "Change"
        anchors.centerIn: parent
        onClicked: {
            root.color = Qt.rgba(Math.random(1), Math.random(1), Math.random(1), 1);
            root.border.width = units.gu(2) * Math.random(1)
            root.border.color = Qt.rgba(Math.random(1), Math.random(1), Math.random(1), 1);

        }
    }
}
//![group-saver]

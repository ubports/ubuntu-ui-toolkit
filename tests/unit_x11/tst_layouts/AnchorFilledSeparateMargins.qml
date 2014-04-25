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
import Ubuntu.Layouts 1.0

Rectangle {
    id: root
    width: 300
    height: 360

    Layouts {
        objectName: "layoutManager"
        id: layoutManager
        anchors.fill: parent

        layouts: [
            ConditionalLayout {
                name: "landscape"
                when: root.width > root.height
                ItemLayout {
                    objectName: "testLayout"
                    anchors.fill: parent
                    item: "test"
                }
            }
        ]

        Rectangle {
            color: "red"
            objectName: "testItem"
            Layouts.item: "test"
            anchors.fill: parent
            anchors.leftMargin: 10
            anchors.topMargin: 20
            anchors.rightMargin: 30
            anchors.bottomMargin: 40
        }
    }
}

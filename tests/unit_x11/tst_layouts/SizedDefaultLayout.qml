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
import Ubuntu.Layouts 1.0

Item {
    objectName: "defaultLayout"
    anchors.fill: parent
    Button {
        objectName: "item1"
        id: label1
        Layouts.item: "item1"
        text: "item1"
        anchors {
            left: parent.left
            right: parent.right
        }
    }
    Button {
        objectName: "item2"
        id: label2
        anchors.bottom: label1.bottom
        Layouts.item: "item2"
        text: "item2"
        anchors {
            left: parent.left
            top: label1.bottom
            right: parent.right
        }
    }
    Button {
        objectName: "item3"
        id: label3
        anchors.bottom: label2.bottom
        Layouts.item: "item3"
        text: "item3"
        anchors {
            left: parent.left
            top: label2.bottom
            right: parent.right
        }
    }
}

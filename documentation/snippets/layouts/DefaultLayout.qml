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
import Ubuntu.Layouts 0.1

Item {
    anchors.fill: parent
    Button {
        id: button1
        text: "Item #1"
        color: "red"
        ConditionalLayout.item: "item1"
        anchors {
            left: parent.left
            top: parent.top
            bottom: parent.bottom
        }
        width: units.gu(15)
    }
    Button {
        id: button2
        text: "Item #2"
        color: "green"
        ConditionalLayout.item: "item2"
        anchors {
            top: parent.top
            left: button1.right
            right: parent.right
        }
        height: units.gu(10)
    }
    Button {
        id: nolayout
        text: "Non-laid out"
        color: "brown"
        anchors {
            top: button2.bottom
            left: button1.right
            right: parent.right
            bottom: button3.top
        }
    }
    Button {
        id: button3
        text: "Item #3"
        color: "blue"
        ConditionalLayout.item: "item3"
        anchors{
            left: button1.right
            right: parent.right
            bottom: parent.bottom
        }
        height: units.gu(10)
    }
}

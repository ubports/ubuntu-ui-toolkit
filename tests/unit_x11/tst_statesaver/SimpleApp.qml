/*
 * Copyright 2014 Canonical Ltd.
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
    property var boolArray: [false, false]
    property var intArray: [1, 2]
    property var realArray: [10.1, 20.2]
    property var stringArray: ["false", "false"]
    id: testItem
    objectName: "testItem"
    StateSaver.properties: "boolArray, intArray, realArray, stringArray"

    Component.onCompleted: UbuntuApplication.applicationName = "SimpleApp"
}

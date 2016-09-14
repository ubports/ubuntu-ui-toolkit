/*
 * Copyright 2016 Canonical Ltd.
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

import QtQuick 2.4

// This component is used in tst_listview_bug1621509.13.qml
Item {
    id: root
    property var myObject
    property int count: 0 // will hold the number of changes to myObject after Component.completed.

    onMyObjectChanged: {
        print("myObject changed to "+myObject)
        count++;
    }
    Component.onCompleted: {
        count = 0;
    }
}

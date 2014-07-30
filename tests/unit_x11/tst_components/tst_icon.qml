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
import QtTest 1.0
import Ubuntu.Components 1.1

Item {
    width: units.gu(50)
    height: units.gu(50)

//    id: root
//    property real listViewHeaderHeight: units.gu(5)

    Row {
        anchors.centerIn: parent
        height: units.gu(10)
        width: childrenRect.width

        Icon {
            // fails loading the icon when suru-icon-theme or
            // libqt5svg5 are not installed
            name: "add"
            height: parent.height
            width: height
        }
        Icon {
            visible: false
            id: icon
            width: visible ? units.gu(10) : 0
            height: visible ? units.gu(10) : 0
            name: "search"
            //                sourceSize {
            //                    width: icon.width
            //                    height: icon.height
            //                }
        }

    }
    Component.onCompleted: icon.visible = true



//    TestCase {
//        name: "HeaderAlignment"
//        when: windowShown

//        function test_ListViewHeaderAlignment_bug1202277() {
//            compare(listView.contentY, -root.listViewHeaderHeight - mainViewHeader.__propagated.header.height,
//                    "ListView header is aligned with the MainView header");
//        }

//        function test_WrappedListViewHeaderAlignment_bug1261907() {
//            compare(wrappedListView.contentY, -root.listViewHeaderHeight - mainViewHeader.__propagated.header.height,
//                    "ListView header inside wrapped Page is aligned with the MainView header");
//        }
//    }
}

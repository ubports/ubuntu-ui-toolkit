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
import QtTest 1.0
import Ubuntu.Test 1.3
import Ubuntu.Components 1.3
import QtQuick.Window 2.1

Item {
    width: units.gu(40)
    height: units.gu(70)

    ListView {
        id: testView
         anchors.fill: parent
         model: 10
         orientation: ListView.Horizontal
         delegate: ListItem {
             width: units.gu(5)
             height: units.gu(5)
             objectName: "listitem" + index
             Button {
                 activeFocusOnTab: false
                 anchors {
                     fill: parent
                     margins: units.gu(1)
                 }
                 text: "Button #" + index
             }
         }
     }

    UbuntuTestCase {
        when: windowShown

        function test_navigation_bug1590005_data() {
            return [
                {tag: "beyond first", index: 0, key: Qt.Key_Left, focusItem: "listitem0"},
                {tag: "beyond last", index: 9, key: Qt.Key_Right, focusItem: "listitem9"},
            ]
        }
        function test_navigation_bug1590005(data) {
            testView.forceActiveFocus();
            testView.currentIndex = data.index;
            waitForRendering(testView, 500);

            keyClick(data.key);
            waitForRendering(testView, 500);
            compare(testView.Window.activeFocusItem, findChild(testView, data.focusItem));
        }
    }
}

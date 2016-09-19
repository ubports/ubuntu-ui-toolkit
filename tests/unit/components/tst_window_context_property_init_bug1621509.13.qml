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

// Ubuntu.Components is not explicitly used below, but its import causes bug #1621509.
import Ubuntu.Components 1.3

TestCase {
    id: testCase
    name: "EmbeddedListView"
    width: 100
    height: 100

    // Bug #1621509 only occurs when we wait for the window to be visible.
    // Forces the components to be completed before the test functions are executed.
    when: windowShown

    Item {
        id: testItem

        // This property changes its value when a new context property is added.
        property var myObject: objectOne
        property int count: 0
        onMyObjectChanged: {
            count++;
        }
        Component.onCompleted: {
            count = 0;
            print("Component completed."); // must print something to reproduce the bug.
        }
        QtObject {
            objectName: "one"
            id: objectOne
        }
    }

    function test_no_property_change_after_completed_bug1621509_bug1610231() {
        compare(testItem.count, 0,
                "The property was changed after the component was completed.");
    }
}

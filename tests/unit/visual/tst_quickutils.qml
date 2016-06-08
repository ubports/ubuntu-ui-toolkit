/*
 * Copyright 2012-2016 Canonical Ltd.
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
import Ubuntu.Components 1.3

Item {
    id: root
    width: units.gu(40)
    height: units.gu(40)

    Column {
        id: focusGroup
        Item {
            objectName: "unfocusableFirst"
        }

        Repeater {
            model: 5
            Item {
                objectName: index == 0 ? "first" : (index == 4 ? "last" : "item" + index)
                activeFocusOnTab: true
            }
        }
        Item {
            objectName: "unfocusableLast"
        }
    }

    TestCase {
        id: test
        name: "QuickUtilsAPI"
        when: windowShown

        function test_rootItem()
        {
            compare(QuickUtils.rootItem(test) != 0, true, "Root item is not null");
        }

        function test_className()
        {
            compare(QuickUtils.className(test), "TestCase", "className for TestCase");
            compare(QuickUtils.className(root), "QQuickItem", "className for Item");
        }

        function test_firstFocusableChild()
        {
            compare(QuickUtils.firstFocusableChild(focusGroup).objectName, "first");
        }

        function test_lastFocusableChild()
        {
            compare(QuickUtils.lastFocusableChild(focusGroup).objectName, "last");
        }
    }
}

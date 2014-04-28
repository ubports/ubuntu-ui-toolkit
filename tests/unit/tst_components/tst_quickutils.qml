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
import Ubuntu.Components.ListItems 1.0

Item {
    id: root
    width: units.gu(40)
    height: units.gu(40)

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
    }
}

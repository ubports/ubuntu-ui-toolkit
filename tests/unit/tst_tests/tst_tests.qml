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
import QtTest 1.0

TestCase {
    Rectangle {
        id: root
    }

    function test_canLoad_UbuntuTests() {
        var obj = Qt.createQmlObject("import QtQuick 2.0; import Ubuntu.Tests 0.1; Rectangle { id: root; Rectangle { objectName: \"myObj\"} Component.onCompleted: Tools.findChild(root,\"myObj\")}",root);
        compare(obj!==undefined,true,"Object was created");
    }
}

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

    function test_canLoad_UbuntuTest() {
        var obj = Qt.createQmlObject("import QtQuick 2.0; import Ubuntu.Test 0.1; Rectangle { id: root; }",root);
        compare(obj!==undefined,true,"Object was created");
    }

    function test_findChild() {
        var obj = Qt.createQmlObject("import QtQuick 2.0; import Ubuntu.Test 0.1; Rectangle { id: root; property variant val; Rectangle { objectName: \"myObj\"} Component.onCompleted: val = Tools.findChild(root,\"myObj\")}",root);
        compare(obj!==undefined,true,"Object was created");
        compare(obj.val.objectName,"myObj","child object was found");
    }
}

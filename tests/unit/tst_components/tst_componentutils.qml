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
import Ubuntu.Components 0.1
// FIXME: When a module contains QML, C++ and JavaScript elements exported,
// we need to use named imports otherwise namespace collision is reported
// by the QML engine. As workaround, we use Theming named import. This is
// needed because Ubuntu.Components library functions are built using this
// named import.
// Bug to watch: https://bugreports.qt-project.org/browse/QTBUG-27645
import Ubuntu.Components 0.1 as Theming

TestCase {
    name: "ComponentUtilsAPI"

    function initTestCase()
    {
        Theme.loadTheme(Qt.resolvedUrl("../../resources/base.qmltheme"));
        compare(Theme.error, "", "Theme loading error");
    }

    function test_hasStyle()
    {
        var result = ComponentUtils.hasStyle(testItem, "prop_baseA_A");
        compare(result, true, "Style property check");

        result = ComponentUtils.hasStyle(testItem, "nonexistent");
        compare(result, false, "Style property check");
    }

    function test_style()
    {
        var result = ComponentUtils.style(testItem, "prop_baseA_A")
        compare(result, "prop_baseA_A", "Existing style property getter")

        result = ComponentUtils.style(testItem, "nonexistent")
        compare(result, undefined, "Non-existent style property getter")

        result = ComponentUtils.style(testItem, "nonexistent", 10)
        compare(result, 10, "Non-existent style property getter with default value")
    }

    Item {
        id: testItem
        // FIXME: see previous
        Theming.ItemStyle.class: "baseA"
    }
}

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

Item {
    width: 400
    height: 400

    Item {
        id: styledItem
        SignalSpy {
            id: spy
            target: parent
            signalName: "styleChanged"
        }
    }

    TestCase {
        name: "ThemeEngineAPI"
        when: windowShown

        function test_inheritance() {
            Theme.loadTheme(Qt.resolvedUrl("../../resources/inheritance.qmltheme"));
            compare(Theme.error, "", "Theme loaded");

            spy.clear();
            styledItem.ItemStyle.class = "basea";
            tryCompare(spy, "count", 1, 100);
            compare(styledItem.ItemStyle.style.propertyDerived, "propertyDerived", "derived");
        }
    }
}

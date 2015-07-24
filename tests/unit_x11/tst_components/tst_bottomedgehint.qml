/*
 * Copyright 2015 Canonical Ltd.
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
import Ubuntu.Test 1.0
import Ubuntu.Components 1.3

MainView {
    id: mainView
    width: units.gu(40)
    height: units.gu(71)

    BottomEdgeHint {
        id: bottomEdgeHint
    }

    UbuntuTestCase {
        name: "BottomEdgeHint"
        when: windowShown

        SignalSpy {
            id: clickSpy
            target: bottomEdgeHint
            signalName: "onClicked"
        }

        function cleanup() {
            bottomEdgeHint.iconName = "";
            bottomEdgeHint.state = "Visible";
            clickSpy.clear();
        }

        function test_0_default_state() {
            compare(bottomEdgeHint.iconName, "");
            compare(bottomEdgeHint.text, "");
            compare(bottomEdgeHint.state, "Visible");
            compare(bottomEdgeHint.width, units.gu(7));
            compare(bottomEdgeHint.height, units.gu(4));
            compare(bottomEdgeHint.y, mainView.height - bottomEdgeHint.height);
            compare(clickSpy.count, 0, "The BottomEdgeHint should not have received a click.");
        }

        function test_with_text() {
            bottomEdgeHint.text = "A Label";
            compare(bottomEdgeHint.text, "A Label");
            verify(bottomEdgeHint.width > units.gu(7));
        }

        function test_hiding() {
            bottomEdgeHint.state = "Hidden";
            tryCompare(bottomEdgeHint, "y", mainView.height - bottomEdgeHint.height + units.gu(2.5));
        }

        function test_clicking() {
            mouseClick(bottomEdgeHint, 0, 0);
            compare(clickSpy.count, 1, "The BottomEdgeHint should have received a click.");
        }
    }
}

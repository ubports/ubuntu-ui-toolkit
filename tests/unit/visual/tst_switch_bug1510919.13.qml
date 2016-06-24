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

import QtQuick 2.4
import Ubuntu.Components 1.3
import QtTest 1.0
import Ubuntu.Test 1.0

MainView {
    width: units.gu(40)
    height: units.gu(71)

    Column {
        CheckBox {
            id: checkBox
        }
        Switch {
            id: switchBox
        }
    }

    UbuntuTestCase {
        name: "CheckAndSwitch"
        when: windowShown

        SignalSpy {
            id: clickSpy
            signalName: "clicked"
        }

        function cleanup() {
            clickSpy.target = null;
            clickSpy.clear();
            checkBox.checked = false;
            switchBox.checked = false;
        }

        function initTestCase() {
            TestExtras.registerTouchDevice();
        }

        function test_checked_in_sync_with_clicked_data() {
            return [
                {tag: "CheckBox with touch", item: checkBox, touch: true},
                {tag: "CheckBox with mouse", item: checkBox, touch: false},
                {tag: "Switch with touch", item: switchBox, touch: true},
                {tag: "Switch with mouse", item: switchBox, touch: false},
            ];
        }
        function test_checked_in_sync_with_clicked(data) {
            var checked = data.item.checked;
            clickSpy.target = data.item;
            if (data.touch) {
                TestExtras.touchClick(0, data.item, centerOf(data.item));
            } else {
                mouseClick(data.item, centerOf(data.item).x, centerOf(data.item).y);
            }
            clickSpy.wait(500);
            compare(data.item.checked, !checked, "checked property shoudl be altered!");
        }
    }
}

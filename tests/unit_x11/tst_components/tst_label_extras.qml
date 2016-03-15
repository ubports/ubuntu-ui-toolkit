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
import QtTest 1.0
import Ubuntu.Test 1.0
import Ubuntu.Components 1.3

Item {
    id: main
    width: units.gu(40)
    height: units.gu(70)

    Component {
        id: labelWithButton
        Label {
            text: "Something"
            Button {
                objectName: "childButton"
                text: "Something else"
                onClicked: visible = false
            }
        }
    }

    Component {
        id: testLabel
        Label {
            text: "Hello Dolly!"
        }
    }

    Loader {
        id: testLoader
    }

    UbuntuTestCase {
        name: "Label13Extras"
        when: windowShown

        function loadTest(component) {
            testLoader.sourceComponent = component;
            tryCompare(testLoader, "status", Loader.Ready, 1000);
            verify(testLoader.item);
            return testLoader.item;
        }

        function cleanup() {
            testLoader.sourceComponent = null;
            waitForRendering(main, 400);
        }

        function test_label_with_button_bug1503901() {
            // this should SEGFAULT on error!
            loadTest(labelWithButton);
        }

        function test_label_color_changes_even_with_same_value_as_default_bug1555784() {
            var test = loadTest(testLabel);

            // override the text color with the same as the default
            test.color = theme.palette.normal.backgroundText;
            // change the color of the palette
            theme.palette.normal.backgroundText = UbuntuColors.blue;
            verify(test.color != theme.palette.normal.backgroundText);
        }
    }
}

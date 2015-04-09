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
    id: main
    width: units.gu(40)
    height: units.gu(71)

    Component.onCompleted: theme.name = "AppTheme"

    Label {
        id: testLabel
    }

    UbuntuTestCase {
        name: "AppTheming"
        when: windowShown

        function test_app_theme_defaults() {
            compare(main.backgroundColor, "#a21e1c", "Theme not loaded?");
            compare(testLabel.color, "#add8e6", "Label color not from application theme.");
        }

        // the following tests should give binding loops which are captured by the runtest.sh
        // and will report failure if received
        function test_no_binding_loop_when_set_to_ambiance()
        {
            main.theme.name = "Ubuntu.Components.Themes.Ambiance";
        }

        function test_reset_theme_should_not_give_binding_loops()
        {
            main.theme.name = undefined;
        }
    }
}

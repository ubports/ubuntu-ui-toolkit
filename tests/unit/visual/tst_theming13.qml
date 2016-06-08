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
import Ubuntu.Components 1.3
import Ubuntu.Test 1.0

MainView {
    id: mainView
    width: units.gu(50)
    height: units.gu(10)

    UbuntuTestCase {
        when: windowShown

        SignalSpy {
            id: themeSpy
            signalName: "paletteChanged"
            target: Theme
        }

        function cleanup() {
            themeSpy.clear();
        }

        function test_backgroundcolor_change_data() {
            return [
                {tag: "Light color", oldTheme: "Ubuntu.Components.Themes.SuruDark", newTheme: "Ubuntu.Components.Themes.Ambiance", color: "white"},
                {tag: "Dark color", oldTheme: "Ubuntu.Components.Themes.Ambiance", newTheme: "Ubuntu.Components.Themes.SuruDark", color: "blue"},
            ];
        }
        function test_backgroundcolor_change(data) {
            if (data.oldTheme !== mainView.theme.name) {
                mainView.theme.name = data.oldTheme;
                themeSpy.wait();
            }
            // change color;
            themeSpy.clear();
            mainView.backgroundColor = data.color;
            themeSpy.wait();
            compare(mainView.theme.name, data.newTheme, "Theme not chnaged");
        }
    }
}

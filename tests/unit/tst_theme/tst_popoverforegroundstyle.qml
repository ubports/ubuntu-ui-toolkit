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
import Ubuntu.Components.Themes.Ambiance 0.1
import Ubuntu.Components.Popups 1.0

TestCase {
    name: "PopupsPopoverAPI"

    function test_show() {
        popOver.show()
    }

    Popover {
        id: popOver

        foregroundStyle: testForegroundStyle
        Text {
            text: "Hello Popover!"
        }
    }

    Component {
        id: testForegroundStyle
        PopoverForegroundStyle {
        }
    }
}

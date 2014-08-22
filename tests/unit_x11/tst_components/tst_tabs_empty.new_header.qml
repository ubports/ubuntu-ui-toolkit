/*
 * Copyright 2014 Canonical Ltd.
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

import QtQuick 2.2
import Ubuntu.Components 1.1
import Ubuntu.Test 1.0

MainView {
    width: units.gu(50)
    height: units.gu(100)

    useDeprecatedToolbar: false

    Tabs {
        id: empty
    }

    UbuntuTestCase {
        name: "EmptyTabsNewHeader"
        when: windowShown

        function test_emptyTabs() {
            compare(empty.selectedTabIndex, -1, "The default value for selectedTabIndex is -1 when there are no tabs");
            compare(empty.selectedTab, null, "The default selected tab is null when there are no tabs");
            compare(empty.currentPage, null, "The default currentPage is null when there are no tabs");
        }
    }
}

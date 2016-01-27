/*
 * Copyright 2014-2015 Canonical Ltd.
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
import Ubuntu.Components 1.3

MainView {
    width: units.gu(70)
    height: units.gu(60)
    objectName: "mainView"

    Tabs {
        id: tabs
        Tab {
            objectName: "tab1"
            title: "Tab1"
            page: Page { }
        }
        Tab {
            objectName: "tab2"
            title: "Tab2"
            page: Page { }
        }
        Tab {
            objectName: "tab3"
            title: "Tab3"
            page: Page { }
        }
    }
}

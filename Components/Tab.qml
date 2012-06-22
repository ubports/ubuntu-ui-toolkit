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

import QtQuick 1.1

/*!
    \qmlclass Tab
    \inqmlmodule UbuntuUIToolkit
    \brief TODO

    \b{This component is under heavy development.}

    Examples: See Tabs.
*/

// Note: In this implementation, Tab cannot be a subclass of QtObject, because
// we need its parent to get the TabGroup and button Row.
Item {
    id: tab

    property string text
    property url iconSource
    property Item page
    property url pageSource
    property bool pagePreloaded

    Loader {
        id: loader

    }

   // Item {
        // hidden from API
       // id: item

       // property Item tabs: tab.parent ? tab.parent : null
       // property Item tabGroup: "tabGroup" in item.tabs ? item.tabs.tabGroup : null
       // property Row buttonRow: "buttonRow" in item.tabs ? item.tabs.buttonRow : null

        /*
        TabButton {
            parent: item.buttonRow
            id: button
            text: tab.text
            iconSource: tab.iconSource
            tab: tab.page
            width: 100
            //height: 30
        }
        */
/*
        Component.onCompleted: {
            if (tab.page) tab.page.parent = item.tabGroup;
            //if (__tabGroup.currentTab == undefined) __tabGroup.currentTab = page;
            print("completed Tab "+text);
            //tabs.Tabs.addTab(tab);
        }
        */
    //}
}

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
*/

QtObject { // TODO: make QtObject, but then I cannot create a TabButton here
    id: tab

    property string text
    property url iconSource
    property Item page
    property url pageSource
    property bool pagePreloaded

//    property Item __tabs: tab.parent ? tab.parent : null
//    property Item __tabGroup: "tabGroup" in __tabs ? __tabs.tabGroup : null
//    property Row __buttonRow:] "buttonRow" in __tabs ? __tabs.buttonRow : null
/*
    TabButton {
        parent: tab.__buttonRow
        id: button
        text: tab.text
        tab: tab.page
        width: 100
        //height: 30
    }

    Component.onCompleted: {
        if (page) page.parent = __tabGroup;
        //if (__tabGroup.currentTab == undefined) __tabGroup.currentTab = page;
        print("completed Tab "+text);
    }
    */
}

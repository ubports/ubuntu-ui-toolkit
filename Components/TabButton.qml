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
    \qmlclass TabButton
    \inqmlmodule UbuntuUIToolkit
    \brief A special button class for controlling tabs.
*/
Button {
    id: tabbutton

    /*!
      \preliminary
      The tab that will be activated by this button.
      The parent of the tab Item must be a TabGroup.
     */
    property Item tab

    /*!
      \preliminary
      True if tab is the selected tab of its tabgroup, false otherwise.
     */
    property bool selected: (__tabGroup !== null) ? (__tabGroup.currentTab == tab): false

    /*!
      \internal
      The parent of the tab property. Must be of type TabGroup.
     */
    property Item __tabGroup: __getTabGroup()

    onClicked: __selectTab()

    /*!
      \internal
      Make the tab associated with this TabButton the current
      tab in its parent TabGroup.
    */
    function __selectTab() {
        if ( (__tabGroup != null) && (tab != null) ) {
                __tabGroup.currentTab = tab;
        } // if
    } // function

    /*!
      \internal
      If the tab has a parent that is a valid TabGroup, then that TabGroup is returned.
      Otherwise null is returned.
     */
    function __getTabGroup() {
        var result = null;
        if (tab !== null) {
            if (tab.parent !== null) {
                if ("currentTab" in tab.parent) {
                    result = tab.parent;
                }
            }
        }
        return result;
    }

    state: {
        if (tabbutton.selected) return "selected";
        else return __getState();
    }

    states: [
        State {
            name: "selected"
            PropertyChanges { target: tabbutton; color: "pink" }
        }

    ]
}

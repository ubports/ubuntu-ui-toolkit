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
ButtonWithForeground {
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
    property bool __selected: (__tabGroup !== null) ? (__tabGroup.currentTab == tab): false

    /*!
      \internal
      The parent of the tab property. Must be of type TabGroup.
     */
    property Item __tabGroup: __getTabGroup()

    height: parent ? parent.height : 50

    /*!
      \internal
      These properties keep track on whether the TabButton is the first or the
      last in the current row or tabs. This changes their appearance (rounded
      borders vs. straight corners for TabButtons that are not first or last).
     */
    property bool __isFirst: parent ? (parent.children[0] == tabbutton) : false
    property bool __isLast: parent ? (parent.children[parent.children.length-1] == tabbutton) : false

    onClicked: __selectTab()

    BorderImage {
        id: background
        z: -1

        anchors.fill: parent
        source: {
            if (__isFirst) {
                return __selected ? "artwork/TabLeftSelected.png" : "artwork/TabLeftUnselected.png"
            } else if (__isLast) {
                return __selected ? "artwork/TabRightSelected.png" : "artwork/TabRightUnselected.png"
            } else {
                return __selected ? "artwork/TabMiddleSelected.png" : "artwork/TabMiddleUnselected.png"
            }
        }

        border { left: __isFirst ? 9 : 1; top: __isFirst || __isLast ? 9 : 2; right: __isLast ? 10 : 2; bottom: 0 }
        horizontalTileMode: BorderImage.Stretch
        verticalTileMode: BorderImage.Stretch
    }


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

    /*
    state: {
        if (tabbutton.__selected) return "selected";
        else return __getState();
    }
    */
    /*
    states: [
        State {
            name: "selected"
            //PropertyChanges { target: tabbutton; color: "pink" }
        }
    ]
    */
}

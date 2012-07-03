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
    \qmlclass tabButton
    \inqmlmodule UbuntuUIToolkit
    \brief Button used in Tab bars.
*/
ButtonWithForeground {
    id: tabButton

    /*!
      \preliminary
      True iff the tab is selected.
      This value is automatically updated by the Tabs object.
     */
    property bool selected: false;

    height: parent ? parent.height : 50

    BorderImage {
        id: background
        z: -1

        property Item allTabs: tabButton.parent
        /*!
          \internal
          These properties keep track on whether the tab button is the first or the
          last in the current row or tabs. This changes their appearance (rounded
          borders vs. straight corners for tabButtons that are not first or last).
         */
        property bool __isFirst: allTabs ? (allTabs.children[0] == tabButton) : false
        property bool __isLast: allTabs ? (allTabs.children[allTabs.children.length-1] == tabButton) : false

        anchors.fill: parent
        source: {
            if (__isFirst) {
                return tabButton.selected ? "artwork/TabLeftSelected.png" : "artwork/TabLeftUnselected.png"
            } else if (__isLast) {
                return tabButton.selected ? "artwork/TabRightSelected.png" : "artwork/TabRightUnselected.png"
            } else {
                return tabButton.selected ? "artwork/TabMiddleSelected.png" : "artwork/TabMiddleUnselected.png"
            }
        }

        border {
            left: __isFirst ? 9 : 1
            top: __isFirst || __isLast ? 9 : 2
            right: __isLast ? 10 : 2
            bottom: 0
        }
        horizontalTileMode: BorderImage.Stretch
        verticalTileMode: BorderImage.Stretch
    }
}

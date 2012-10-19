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

/*!
    \qmltype tabButton
    \inqmlmodule Ubuntu.Components 0.1
    \ingroup ubuntu
    \brief Button used in Tab bars.
*/
ButtonWithForeground {
    id: tabButton

    /*!
      \preliminary
      True if the tab is selected.
      This value is automatically updated by the Tabs object.
     */
    property bool selected: false;

    height: parent ? parent.height : units.gu(6)
    textColor: "#757373"

    /*!
       \internal
       These properties keep track on whether the tab button is the first or the
       last in the current row or tabs. This changes their appearance (rounded
       borders vs. straight corners for tabButtons that are not first or last).
     */
    property bool __isFirst: false
    /*!
      \internal
     */
    property bool __isLast: false


    BorderImage {
        id: background
        z: -1

        property Item allTabs: tabButton.parent

        anchors.fill: parent
        source: {
            if (tabButton.__isFirst) {
                return tabButton.selected ? "artwork/TabLeftSelected.sci" : "artwork/TabLeftUnselected.sci"
            } else if (tabButton.__isLast) {
                return tabButton.selected ? "artwork/TabRightSelected.sci" : "artwork/TabRightUnselected.sci"
            } else {
                return tabButton.selected ? "artwork/TabMiddleSelected.sci" : "artwork/TabMiddleUnselected.sci"
            }
        }
    }
}

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
    \brief A special Button class for controlling tabs
*/
ColoredButton {
    id: button

    /*!
      \preliminary
      The tab that will be activated by this button.
     */
    property Item tab

    /*!
      \internal
     */
    // TODO: check the type of tab.parent? (here or in __selectTab)
    property Item __tabGroup: tab !== null ? tab.parent : null

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
}

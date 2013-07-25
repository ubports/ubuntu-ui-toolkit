/*
 * Copyright 2013 Canonical Ltd.
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
    \qmltype TabBar
    \inqmlmodule Ubuntu.Components 0.1
    \ingroup ubuntu
    \brief Tab bar that will be shown in the header when \l Tabs is active.
        This component does not need to be instantiated by the developer, it is
        automatically created by the \l Tabs.
*/
StyledItem {
    id: tabBar
    anchors.fill: parent
    visible: parent

    // HeaderStyle binds parent when the TabBar should be visible
    parent: null

    /*!
      The \l Tabs item that tab bar belongs to.
      Will be automatically set by \l Tabs when the TabBar is created.
     */
    // tabsItem is of type Tabs, but using that type would cause an include loop
    property Item tabsItem

    /*!
      An inactive tab bar only displays the currently selected tab,
      and an active tab bar can be interacted with to select a tab.
     */
    // TODO: make readonly, or ensure the TabBar is updated when this property is updated.
    property bool active: alwaysActive

    /*!
      Do not deactivate the tab bar after a specified idle time or when the user selects a new tab.
      Off by default.
     */
    property bool alwaysActive: false

    /*!
      Automatically activate the tab bar when \l alwaysActive is set.
     */
    onAlwaysActiveChanged: {
        if (tabBar.alwaysActive) active = true;
    }

    style: Theme.createStyleComponent("TabBarStyle.qml", tabBar)
}

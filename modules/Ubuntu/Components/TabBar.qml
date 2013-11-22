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

    /*!
      \deprecated
      The \l Tabs item that tab bar belongs to.
      Will be automatically set by \l Tabs when the TabBar is created.
     */
    // tabsItem is of type Tabs, but using that type would cause an include loop
    property Item tabsItem
    /*! \internal */
    onTabsItemChanged: console.error("tabsItem property is deprecated. TabBar functionality no longer requires it.")

    /*!
      The model containing the tabs to be controlled by the TabBar. The tabs are
      visualized by the style, displaying controlling elements based on the data
      specified by the roles. The default style mandates the existence of either
      the \b title or \b tab role, but different styles may require to have other
      roles (e.g. image, color). The order the role existence is checked is also
      determined by the style component, Default style checks the existence of the
      \b tab role first, and if not defined will use the \b title role.
      */
    property ListModel model

    /*!
      An inactive tab bar only displays the currently selected tab,
      and an active tab bar can be interacted with to select a tab.
     */
    property bool selectionMode: true

    /*!
      The property holds the index of the selected Tab item.
      */
    property int selectedIndex: (model && model.count > 0) ? 0 : -1

    /*!
      Do not deactivate the tab bar after a specified idle time or when the user selects a new tab.
      Off by default.
     */
    property bool alwaysSelectionMode: false

    /*!
      Automatically activate the tab bar when \l alwaysSelectionMode is set.
     */
    onAlwaysSelectionModeChanged: {
        if (tabBar.alwaysSelectionMode) selectionMode = true;
    }

    /*!
      Show animations when the state changes. Default: true.
      */
    property bool animate: true

    /*! \internal
      Animation is only enabled after completion of the TabBar component to
      avoid animating tabs when first showing them.
     */
    // FIXME: It would be better to deal with this in the style, but because
    //  of the order the properties in the style implementation are updated,
    //  that always resulted in some animation showing at startup.
    Component.onCompleted: {
        if (__styleInstance.hasOwnProperty("animate")) {
            __styleInstance.animate = Qt.binding(function() {
                return tabBar.animate;
            });
        }
    }

    implicitHeight: units.gu(7.5)

    style: Theme.createStyleComponent("TabBarStyle.qml", tabBar)
}

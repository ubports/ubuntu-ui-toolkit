/*
 * Copyright 2015 Canonical Ltd.
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

import QtQuick 2.4
import Ubuntu.Components 1.3 as Toolkit

/*!
    \qmltype TabBar
    \inqmlmodule Ubuntu.Components
    \ingroup ubuntu
    \deprecated
    \brief Tab bar that will be shown in the header when \l Tabs is active.
        This component does not need to be instantiated by the developer, it is
        automatically created by the \l Tabs.
*/
Toolkit.StyledItem {
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
    property var model: null

    /*!
      The user is interacting with the tab bar.
      Depends on the style pressed property.
     */
    readonly property bool pressed: __styleInstance !== null && __styleInstance.hasOwnProperty("pressed") ?
                                        __styleInstance.pressed : false

    /*!
      An inactive tab bar only displays the currently selected tab,
      and an active tab bar can be interacted with to select a tab.
     */
    property bool selectionMode: true

    /*!
      The property holds the index of the selected Tab item.
      Note: Setting this property is DEPRECATED. Set the selectedIndex of the model instead.
      */
    property int selectedIndex: (model && internal.modelChecked) ? model.selectedIndex : -1

    /*! \internal */
    onSelectedIndexChanged: {
        if (!model) return;
        if (tabBar.selectedIndex !== model.selectedIndex) {
            console.warn("Setting TabBar.selectedIndex is DEPRECATED. Set selectedIndex of the model instead");
            tabBar.selectedIndex = Qt.binding(function() { return (model && internal.modelChecked) ? model.selectedIndex : -1 });
        }
    }

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

    implicitHeight: units.gu(7.5)
    activeFocusOnPress: true
    styleName: "TabBarStyle"

    QtObject {
        id: internal

        property bool modelChecked: true;

        // returns true if the roles are good, false otherwise.
        function checkRoles() {
            if (tabBar.model.count <= 0)
                return false;

            modelChecked = true;
            var f = tabBar.model.get(0);
            if (f.tab === undefined && f.title === undefined) {
                console.error("TabBar model must provide either tab or title role.");
                tabBar.model = null;
                return false;
            }
            if (f.tab !== undefined && f.tab.title === undefined) {
                console.error("TabBar model's tab role must have title property.");
                tabBar.model = null;
                return false;
            }
            return true;
        }
    }

    /*! \internal */
    onModelChanged: {
        internal.modelChecked = true;

        if (!model)
            return;

        if (!model.hasOwnProperty("selectedIndex")) {
            console.error("TabBar model must have selectedIndex property defined.");
            tabBar.model = null;
            return;
        }

        if (!model.hasOwnProperty("count")) {
            console.error("TabBar model must have count property defined.");
            tabBar.model = null;
            return;
        }

        if (!model.hasOwnProperty("get")) {
            console.error("TabBar model must provide get() function.");
            tabBar.model = null;
            return;
        }

        if (model.count > 0) {
            if (internal.checkRoles()) {
                model.selectedIndex = Math.max(Math.min(model.selectedIndex, model.count - 1), 0);
            }
        } else {
            internal.modelChecked = false;
        }
    }

    Connections {
        target: !internal.modelChecked ? tabBar.model : null
        onCountChanged: internal.checkRoles();
    }
}

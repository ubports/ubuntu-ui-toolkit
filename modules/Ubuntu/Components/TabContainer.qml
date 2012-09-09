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
    \qmlclass TabContainer
    \inqmlmodule Ubuntu.Components 0.1
    \brief A container for Tab objects.
    This class contains a \l tabs property that is automatically updated
    to contain all of the container's children that are of type Tab.

    \b{This component is under heavy development.}

    Examples: See subclasses.
*/
Item {
    id: tabContainer

    /*!
      \internal
      Update the tabs list.
     */
    function __updateTabs() {
        var tabArray = [];
        var item;
        for (var i=0; i < tabContainer.children.length; i++) {
            item = tabContainer.children[i];
            if (item.__isTab === true) tabArray.push(item);
        }
        tabContainer.tabs = tabArray;
    }

    /*!
      \preliminary
      The list of all the children that are a Tab.
      It is automatically updated when children are added or removed.
     */
    property list<Tab> tabs

    /*!
      \internal
     */
    onChildrenChanged: __updateTabs()
}

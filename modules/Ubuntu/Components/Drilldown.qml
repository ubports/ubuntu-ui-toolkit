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
import "ListItems" as ListItem

/*!
    \qmlclass Drilldown
    \inqmlmodule Ubuntu.Components 0.1
    \brief TODO

    \b{This component is under heavy development.}

    Examples: TODO
*/
Page {
    id: drilldown

//    function __push(page) {
//        if (__pageStack) __pageStack.push(page);
//    }

    /*!
      \internal
      Set by the page stack when the drilldown is pushed
     */
//    property alias __pageStack: pageList.pageStack // TODO: move to page?
//    property alias pageStack: pageList.pageStack
    property PageStack pageStack
//    onPageStackChanged: pageList.pageStack = pageStack

    height: childrenRect.height

    default property alias children: pageList.children

    PageList {
        id: pageList
        pageStack: drilldown.pageStack
    }

    contents: pageList
}

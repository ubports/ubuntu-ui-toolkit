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

// internal class
Item {
    id: pageContainer

    /*!
      \internal
      Update the pages list.
     */
    function __updatePages() {
        var pageArray = new Array();
        var item;
        for (var i=0; i < pageContainer.children.length; i++) {
            item = pageContainer.children[i];
            if (item.__isPage === true) pageArray.push(item);
        }
        pageContainer.pages = pageArray;
    }

    /*!
      \preliminary
      The list of all the children that are a Page.
     */
    property list<Page> pages

    onChildrenChanged: __updatePages()
}

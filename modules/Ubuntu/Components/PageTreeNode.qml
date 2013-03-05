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
import "pageUtils.js" as PageUtils
/*!
    \internal
    \qmltype PageTreeNode
    \inqmlmodule Ubuntu.Components 0.1
    \ingroup ubuntu
*/
Item {
    id: node

    /*!
      \internal
      Used to determine whether an Item is a Page.
     */
    property bool __isPage: true

    onParentChanged: internal.updatePageTree()

    /*!
      The node is active. When a node becomes active, it updates the
      activePage property of its parent node.
      True by default, but inside a \l PageStack or \l Tabs Item, the
      active property of all \l Page Items except one will be set to false.
     */
//    property bool active: true

    /*!
      The active child node in the page tree.
     */
    property Item activeChildNode: null

    /*!
      The header of the node. Propagates down from the root node.
     */
    property Header header: internal.parentNode ? internal.parentNode.header : null
    readonly property real headerHeight: header ? header.height : 0
    // TODO: Take the same approach with the toolbar, as with the header.


    Item {
        id: internal
        property var parentNode: null
        /*!
          Find the parent node.
         */
        function updatePageTree() {
            internal.parentNode = PageUtils.getParentPageTreeNode(node);
            internal.updateParentNode();
        }

        /*!
          Update the activePage property of the parent node.
         */
        function updateParentNode() {
//            if (internal.parentNode && node.active) {
//                internal.parentNode.activeChildNode = node;
//            }
            if (internal.parentNode) {
                internal.parentNode.activeChildNode = node;
            }
        }
    }
}

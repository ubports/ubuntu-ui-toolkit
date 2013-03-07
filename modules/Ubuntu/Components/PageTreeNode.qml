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
    property bool __isPageTreeNode: true

    onParentChanged: internal.updatePageTree()
    Component.onCompleted: internal.updatePageTree()

    /*!
      The child nodes in the page tree.
     */
    property var childNodes: []

    /*!
      The header of the node. Propagates down from the root node.
     */
    property Header header: internal.parentNode ? internal.parentNode.header : null

    /*!
      The toolbar of the node. Propagates down from the root node.
     */
    property Toolbar toolbar: internal.parentNode ? internal.parentNode.toolbar : null


    // TODO: document
    property bool active: internal.parentNode ? internal.parentNode.active : false

    // TODO: document
    property Item pageStack: internal.parentNode ? internal.parentNode.pageStack : null

    property alias parentNode: internal.parentNode
    Item {
        id: internal
        property Item parentNode: null

        function isPageTreeNode(object) {
            return (object && object.hasOwnProperty("__isPageTreeNode") && object.__isPageTreeNode);
        }

        /*!
          Return the parent node in the PageTreeNode, or null if the item is the root node or invalid.
         */
        function getParentPageTreeNode(item) {
            var node = null;
            if (item) {
                var i = item.parent;
                while (i && !node) {
                    if (internal.isPageTreeNode(i)) node = i;
                    i = i.parent;
                }
            }
            return node;
        }

        /*!
          Find the parent node.
         */
        function updatePageTree() {
            // TODO: uncomment and make this work.
//            if (internal.parentNode) {
//                // remove node from the list of childNodes
//                var index = internal.parentNode.childNodes.indexOf(node);
//                internal.parentNode.childNodes.splice(index, 1);
//            }
            internal.parentNode = internal.getParentPageTreeNode(node);
            if (internal.parentNode) {
                internal.parentNode.childNodes.push(node);
            }
        }
    }
}

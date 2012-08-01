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
import "stack.js" as Stack

/*!
    \qmlclass PageStack
    \inqmlmodule Ubuntu.Components 0.1
    \brief TODO

    Examples: TODO
*/

Item {
    id: pageStack

    /*!
      \internal
      To give the children the correct parent, which can be used
      for anchoring.
    */
    default property alias children: pages.children

    function push(page) {
        Stack.stack.push(page);
        pages.stackUpdated();
    }

    function pop() {
        Stack.stack.pop();
        pages.stackUpdated();
    }

    Item {
        id: pages
        anchors {
            fill: parent
            topMargin: 50 // TODO: later use ToolBar.height
        }

        // In QtQuick1, this is impossible.
        // QtQuick2 introduces var type that can be JS variables (not possible for variant).
        //property var stack: new Stack.Stack()
        // Instead of this, the variable is created in stack.js,
        // and can be accessed as Stack.stack
        // FIXME: After switching to QtQuick2, we can simply use a stack
        // variable as pages.stack

        property Item currentPage

        function stackUpdated() {
            if (pages.currentPage) pages.currentPage.visible = false;
            pages.currentPage = Stack.stack.top();
            if (pages.currentPage) {
//                pages.currentPage.parent = pages;
                pages.currentPage.visible = true;
                pages.currentPage.anchors.verticalCenter = pages.verticalCenter;
                pages.currentPage.anchors.right = pages.currentPage.parent.right;
            }
        }
    }
}

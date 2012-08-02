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

    property Page rootPage

    /*!
      \internal
      To give the children the correct parent, which can be used
      for anchoring.
    */
//    default property alias children: pages.children

    function push(page) {
        if (Stack.stack.size > 0) Stack.stack.top.active = false;
        Stack.stack.push(page);
        //pages.stackUpdated();
        page.contentsParent = pageContents;
        page.active = true; //(page === Stack.stack.top());
    }

    function pop() {
        if (Stack.stack.size > 1) {
            Stack.stack.top.active = false;
            Stack.stack.pop();
            //pages.stackUpdated();
        }
        Stack.stack.top.active = true;
    }

    // toolbar placeholder
    Rectangle {
        id: toolBar
        color: "darkgrey"
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
        }
        height: 20
    }

    Item {
        id: pageContents
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
            top: toolBar.bottom
        }

        // In QtQuick1, this is impossible.
        // QtQuick2 introduces var type that can be JS variables (not possible for variant).
        //property var stack: new Stack.Stack()
        // Instead of this, the variable is created in stack.js,
        // and can be accessed as Stack.stack
        // FIXME: After switching to QtQuick2, we can simply use a stack
        // variable as pages.stack

    }

    Component.onCompleted: pageStack.push(rootPage)
}

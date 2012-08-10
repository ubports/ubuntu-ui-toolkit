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

    property bool showToolBar: true

    default property Item rootPage
    onRootPageChanged: pageStack.push(rootPage) // TODO: clear the stack, and then push.

    //        // In QtQuick1, this is impossible.
    //        // QtQuick2 introduces var type that can be JS variables (not possible for variant).
    //        //property var stack: new Stack.Stack()
    //        // Instead of this, the variable is created in stack.js,
    //        // and can be accessed as Stack.stack
    //        // FIXME: After switching to QtQuick2, we can simply use a stack
    //        // variable as pages.stack

    function push(page) {
        if (page.__isPage !== true) return;
        if (Stack.stack.size > 0) Stack.stack.top.active = false;
        Stack.stack.push(page);
        page.contentsParent = pageContents;
        // for the drilldown:
        if (page.hasOwnProperty("__pageStack")) page.__pageStack = pageStack;
        page.active = true;
        toolBar.update();
    }

    function pop() {
        if (Stack.stack.size > 1) {
            Stack.stack.top.active = false;
            Stack.stack.pop();
        }
        Stack.stack.top.active = true;
        toolBar.update();
    }

    /*!
      \internal
      Used to determine whether the back button is active.
     */
    property int __stackSize: 0

    Item {
        parent: pageStack
        anchors.fill: parent
        // toolbar placeholder
        Rectangle {
            id: toolBar

            function update() {
                var stackSize = Stack.stack.size;
                if (stackSize > 0) {
                    if (stackSize > 1) {
                        backButton.enabled = true;
                    } else {
                        backButton.enabled = false;
                    }
                    toolBarTitle.text = Stack.stack.top.title;
                } else {
                    backButton.enabled = false
                }
            }

            visible: pageStack.showToolBar
            color: "#222222"
            anchors {
                left: parent.left
                right: parent.right
                top: parent.top
            }
            height: visible ? 40 : 0
            Button {
                id: backButton
                anchors {
                    left: parent.left
                    top: parent.top
                    bottom: parent.bottom
                    margins: 5
                }
                text: "Back"
                darkBorder: true
                visible: true
                enabled: pageStack.__stackSize > 1
                onClicked: pageStack.pop()
            }
            TextCustom {
                id: toolBarTitle
                anchors {
                    left: backButton.right
                    top: parent.top
                    bottom: parent.bottom
                    margins: 5
                    right: parent.right
                }
                color: "white"
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
            }
        }

        Rectangle {
            color: "#eeeeee"
            id: pageContents
            anchors {
                left: parent.left
                right: parent.right
                bottom: parent.bottom
                top: toolBar.bottom
            }
        }
    }
}

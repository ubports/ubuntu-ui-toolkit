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
    \brief A stack of \l Page items that is used for inter-Page navigation.
        Pages on the stack can be popped, and new Pages can be pushed.

    Example:
    \qml
    PageStack {
        id: pageStack
        anchors.fill: parent

        initialPage: firstPage

        Page {
            id: firstPage
            title: "Optional title"
            contents: Column {
                anchors.centerIn: parent
                Text {
                    text: "This is the root page!"
                }
                Row {
                    Button {
                        text: "Orange"
                        onClicked: pageStack.push(orange)
                    }
                    Button {
                        text: "blue"
                        onClicked: pageStack.push(blue)
                    }
                }
            }
        }

        Page {
            id: orange
            title: "Orange"
            contents: Rectangle {
                color: "orange"
                width: 250
                height: 250
                anchors.centerIn: parent
                Button {
                    text: "go back"
                    anchors.centerIn: parent
                    onClicked: pageStack.pop()
                }
            }
        }

        Page {
            id: blue
            title: "Blue"
            contents: Rectangle {
                id: pink
                color: "darkblue"
                radius: 5
                width: 150
                height: 150
                anchors.centerIn: parent
                Button {
                    text: "back"
                    anchors.centerIn: parent
                    onClicked: pageStack.pop()
                    darkBorder: true
                }
            }
        }
    }
    \endqml
*/

Item {
    id: pageStack

    width: parent ? parent.width : undefined
    /*!
      \preliminary
      Show a toolbar at the top of the page stack which shows a back button
      to pop the top, and the title of the current page on top.
     */
    property alias showToolbar: toolbar.visible


    /*!
      \preliminary
      The initial page on the stack.
     */
    property Item initialPage
    onInitialPageChanged: {
        Stack.stack.clear();
        pageStack.push(initialPage);
    }

    // FIXME: After switching to QtQuick2, use a var stack property and instead of
    // Stack.stack in this class we can refer to that property.
    //property var stack: new Stack.Stack()

    /*!
      \preliminary
      Push a page to the stack.
     */
    function push(page) {
        if (page.__isPage !== true) {
            print("WARNING: Trying to push an object to the PageStack that is not a Page. Ignoring.");
            return;
        }
        if (Stack.stack.size() > 0) Stack.stack.top().active = false;
        Stack.stack.push(page);
        page.contentsParent = pageContents;
        page.active = true;
        contents.updateToolbar();
    }

    /*!
      \preliminary
      Pop the top item from the stack if the stack size is at least 1.
      Do not do anything if 0 or 1 items are on the stack.
     */
    function pop() {
        if (Stack.stack.size() > 1) {
            Stack.stack.top().active = false;
            Stack.stack.pop();
        } else {
            print("WARNING: Trying to pop a PageStack with "+Stack.stack.size()+" Pages. Ignoring.");
        }

        Stack.stack.top().active = true;
        contents.updateToolbar();
    }

    Item {
        id: contents
        parent: pageStack
        anchors.fill: parent

        Toolbar {
            id: toolbar
            anchors {
                left: parent.left
                right: parent.right
                top: parent.top
            }

            pageStack: pageStack
        }

        Rectangle {
            color: "#eeeeee"
            id: pageContents
            anchors {
                left: parent.left
                right: parent.right
                bottom: parent.bottom
                top: toolbar.bottom
            }
        }

        function updateToolbar() {
            var stackSize = Stack.stack.size();
            if (stackSize > 1) {
                toolbar.showBackButton = true
            } else {
                toolbar.showBackButton = false
            }
            if (stackSize > 0) {
                toolbar.title = Stack.stack.top().title;
            } else {
                toolbar.title = "";
            }
        }
    }
}

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

    Example: TODO
    \qml
    \endqml
*/

Item {
    id: pageStack

    width: parent ? parent.width : undefined
    /*!
      \preliminary
      Show a header bar at the top of the page stack which shows a back button
      to pop the top, and the title of the current page on top.
     */
    property alias showHeader: header.visible

    /*!
      \internal
      This allows to define the pages inside the PageStack without showing them.
     */
    default property alias children: pages.children
    Item {
        id: pages
        visible: false
    }

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
      \internal
      Create a PageWrapper for the specified page.
     */
    function __createWrapper(page) {
        var wrapperComponent = Qt.createComponent("PageWrapper.qml");
        // TODO: cache the component?
        var wrapperObject = wrapperComponent.createObject(pageContents);
        wrapperObject.reference = page;
        wrapperObject.parent = pageContents;
        return wrapperObject;
    }

    /*!
      \preliminary
      Push a page to the stack.
     */
    function push(page) {
        if (Stack.stack.size() > 0) Stack.stack.top().active = false;

        Stack.stack.push(__createWrapper(page));
        Stack.stack.top().active = true;

        contents.updateHeader();
    }

    /*!
      \preliminary
      Pop the top item from the stack if the stack size is at least 1.
      Do not do anything if 0 or 1 items are on the stack.
     */
    function pop() {
        if (Stack.stack.size() <= 1) {
            print("WARNING: Trying to pop a PageStack with "+Stack.stack.size()+" Pages. Ignoring.");
            return;
        }

        Stack.stack.top().active = false;
        Stack.stack.pop();
        Stack.stack.top().active = true;

        contents.updateHeader();
    }

    Item {
        id: contents
        parent: pageStack
        anchors.fill: parent

        Header {
            id: header
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
                top: header.bottom
            }
        }

        function updateHeader() {
            var stackSize = Stack.stack.size();
            if (stackSize > 1) {
                header.showBackButton = true
            } else {
                header.showBackButton = false
            }
            if (stackSize > 0) {
                var item = Stack.stack.top().object;
                if (item.__isPage === true) header.title = item.title;
                else header.title = "";
            } else {
                header.title = "";
            }
        }
    }
}

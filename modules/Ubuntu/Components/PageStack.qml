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
import "stack.js" as Stack

/*!
    \qmltype PageStack
    \inqmlmodule Ubuntu.Components 0.1
    \ingroup ubuntu
    \brief A stack of \l Page items that is used for inter-Page navigation.
        Pages on the stack can be popped, and new Pages can be pushed.
        The page on top of the stack is the visible one.
        Any non-Page Item that you want to use with PageStack should be created
        with its visible property set to false.

    Example:
    \qml
        import Ubuntu.Components 0.1
        import Ubuntu.Components.ListItems 0.1 as ListItem
        MainView {
            PageStack {
                id: pageStack
                Component.onCompleted: pageStack.push(page0)

                Page {
                    id: page0
                    title: "Root page"

                    Column {
                        anchors.fill: parent
                        ListItem.Standard {
                            text: "Page one"
                            onClicked: pageStack.push(rect, {fillColor: "red"})
                            progression: true
                        }
                        ListItem.Standard {
                            text: "Page two (external)"
                            onClicked: pageStack.push(Qt.resolvedUrl("MyCustomPage.qml"))
                            progression: true
                        }
                    }
                }

                Page {
                    id: fillColor
                    property alias color: rect.color
                    Rectangle {
                        id: rect
                        anchors.fill: parent
                        visible: false
                    }
                }
            }
        }
    \endqml

    Use PageStack inside \l MainView and push \l Page items onto the stack to ensure
    automatic header and toolbar.
*/

PageTreeNode {
    id: pageStack
    anchors.fill: parent

    /*!
      \preliminary
      The current size of the stack
     */
    //FIXME: would prefer this be readonly, but readonly properties are only bound at
    //initialisation. Trying to update it in push or pop fails. Not sure how to fix.
    property int depth: 0

    /*!
      \preliminary
      The currently active page
     */
    property Item currentPage

    /*!
      \internal
      The instance of the stack from javascript
     */
    property var stack: new Stack.Stack()

    /*!
      \internal
      Create a PageWrapper for the specified page.
     */
    function __createWrapper(page, properties) {
        var wrapperComponent = Qt.createComponent("PageWrapper.qml");
        var wrapperObject = wrapperComponent.createObject(pageStack);
        wrapperObject.reference = page;
        wrapperObject.pageStack = pageStack;
        wrapperObject.properties = properties;
        return wrapperObject;
    }

    /*!
      \preliminary
      Push a page to the stack, and apply the given (optional) properties to the page.
     */
    function push(page, properties) {
        if (stack.size() > 0) stack.top().active = false;
        stack.push(__createWrapper(page, properties));
        stack.top().active = true;

        __stackUpdated();
    }

    /*!
      \preliminary
      Pop the top item from the stack if the stack size is at least 1.
      Do not do anything if 0 or 1 items are on the stack.
     */
    function pop() {
        if (stack.size() < 1) {
            print("WARNING: Trying to pop an empty PageStack. Ignoring.");
            return;
        }
        stack.top().active = false;
        if (stack.top().canDestroy) stack.top().destroyObject();
        stack.pop();
        __stackUpdated();

        if (stack.size() > 0) stack.top().active = true;

    }

    /*!
      \preliminary
      Deactivate the active page and clear the stack.
     */
    function clear() {
        while (stack.size() > 0) {
            stack.top().active = false;
            if (stack.top().canDestroy) stack.top().destroyObject();
            stack.pop();
        }
        __stackUpdated();
    }

    /*!
      \internal
     */
    function __stackUpdated() {
        pageStack.depth =+ stack.size();
        if (pageStack.depth > 0) currentPage = stack.top().object;
        else currentPage = null;
    }
}

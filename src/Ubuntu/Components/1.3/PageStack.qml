/*
 * Copyright 2012-2015 Canonical Ltd.
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

import QtQuick 2.4
import "../1.2/stack.js" as Stack
import Ubuntu.Components 1.3
import Ubuntu.Components.Private 1.3

/*!
    \qmltype PageStack
    \inqmlmodule Ubuntu.Components
    \ingroup ubuntu
    \brief A stack of \l Page items that is used for inter-Page navigation.
        Pages on the stack can be popped, and new Pages can be pushed.
        The page on top of the stack is the visible one.

    PageStack should be used inside a \l MainView in order to automatically add
    a header and toolbar to control the stack. The PageStack will automatically
    set the header title to the title of the \l Page that is currently on top
    of the stack, and the tools of the toolbar to the tools of the \l Page on top
    of the stack. When more than one Pages are on the stack, the toolbar will
    automatically feature a back-button that pop the stack when triggered.

    The anchors of the PageStack are set to fill its parent by default. To use
    left/right/top/bottom anchors, explicitly set anchors.fill of the PageStack to
    undefined:
    \qml
        import QtQuick 2.4
        import Ubuntu.Components 1.3

        MainView {
            width: units.gu(40)
            height: units.gu(71)

            PageStack {
                id: mainStack
                anchors {
                    fill: undefined // unset the default to make the other anchors work
                    left: parent.left
                    right: parent.right
                    top: parent.top
                    bottom: rect.top
                }
            }

            Rectangle {
                id: rect
                color: UbuntuColors.red
                anchors {
                    left: parent.left
                    right: parent.right
                    bottom: parent.bottom
                }
                height: units.gu(10)
            }

            Component.onCompleted: mainStack.push(Qt.resolvedUrl("MyPage.qml"))
        }
    \endqml

    Pages that are defined inside the PageStack must initially set their visibility
    to false to avoid the pages occluding the PageStack before they are pushed.
    When pushing a \l Page, its visibility is automatically updated.

    Example:
    \qml
        import QtQuick 2.4
        import Ubuntu.Components 1.3
        import Ubuntu.Components.ListItems 1.3 as ListItem

        MainView {
            width: units.gu(48)
            height: units.gu(60)

            PageStack {
                id: pageStack
                Component.onCompleted: push(page0)

                Page {
                    id: page0
                    title: i18n.tr("Root page")
                    visible: false

                    Column {
                        anchors.fill: parent
                        ListItem.Standard {
                            text: i18n.tr("Page one")
                            onClicked: pageStack.push(page1, {color: UbuntuColors.orange})
                            progression: true
                        }
                        ListItem.Standard {
                            text: i18n.tr("External page")
                            onClicked: pageStack.push(Qt.resolvedUrl("MyCustomPage.qml"))
                            progression: true
                        }
                    }
                }

                Page {
                    title: "Rectangle"
                    id: page1
                    visible: false
                    property alias color: rectangle.color
                    Rectangle {
                        id: rectangle
                        anchors {
                            fill: parent
                            margins: units.gu(5)
                        }
                    }
                }
            }
        }
    \endqml
    As shown in the example above, the push() function can take an Item, Component or URL as input.
*/

PageTreeNode {
    id: pageStack
    anchors.fill: parent

    /*!
      \internal
      Please do not use this property any more. \l MainView now has a header
      property that controls when the header is shown/hidden.
     */
    property bool __showHeader: true
    QtObject {
        property alias showHeader: pageStack.__showHeader
        onShowHeaderChanged: print("__showHeader is deprecated. Do not use it.")
    }

    /*!
      The current size of the stack
     */
    //FIXME: would prefer this be readonly, but readonly properties are only bound at
    //initialisation. Trying to update it in push or pop fails. Not sure how to fix.
    property int depth: 0

    /*!
      The currently active page
     */
    property Item currentPage: null

    /*!
      Push a page to the stack, and apply the given (optional) properties to the page.
      The pushed page may be an Item, Component or URL.
      The function returns the Item that was pushed, or the Item that was created from
      the Component or URL. Depending on the animation of the header, the returned
      Page may or may not be active and on top of the PageStack yet.
     */
    function push(page, properties) {
        internal.finishPreviousAction();
        internal.pageWrapper = internal.createWrapper(page, properties);
        var pageObject = internal.pageWrapper.object;

        if (internal.animateHeader && internal.stack.size() > 0) {
            internal.headStyle.animateOutFinished.connect(internal.pushWrapperObject);
            internal.headStyle.animateOut();
        } else {
            internal.pushWrapperObject();
        }
        // set the back action for Page.header:
        if (pageObject && pageObject.hasOwnProperty("header") && pageObject.header &&
                pageObject.header.hasOwnProperty("navigationActions")) {
            // Page.header is an instance of PageHeader.
            pageObject.header.navigationActions = [ backAction ];
        }

        return pageObject;
    }

    /*!
      Pop the top item from the stack if the stack size is at least 1.
      Do not do anything if 0 items are on the stack.
     */
    function pop() {
        internal.finishPreviousAction();
        if (internal.stack.size() < 1) {
            print("WARNING: Trying to pop an empty PageStack. Ignoring.");
            return;
        }
        // do not animate if there is no page to animate back in after popping
        if (internal.animateHeader && internal.stack.size() > 1) {
            internal.headStyle.animateOutFinished.connect(internal.popAndDestroy);
            internal.headStyle.animateOut();
        } else {
            internal.popAndDestroy();
        }
    }

    /*!
      Deactivate the active page and clear the stack.
     */
    function clear() {
        internal.finishPreviousAction();
        while (internal.stack.size() > 0) {
            internal.stack.top().active = false;
            if (internal.stack.top().canDestroy) internal.stack.top().destroyObject();
            internal.stack.pop();
        }
        internal.stackUpdated();
    }

    Action {
        // used when the Page has a Page.header property set.
        id: backAction
        visible: pageStack.depth > 1
        iconName: "back"
        text: "Back"
        onTriggered: pageStack.pop()
        objectName: "pagestack_back_action"
    }

    Component {
        id: pageWrapperComponent
        PageWrapper{
        }
    }

    QtObject {
        id: internal
        property Item headStyle: (pageStack.__propagated
                                      && pageStack.__propagated.header
                                      && pageStack.__propagated.header.__styleInstance)
                                    ? pageStack.__propagated.header.__styleInstance
                                    : null

        property bool animateHeader: pageStack.__propagated &&
                                     pageStack.__propagated.hasOwnProperty("animateHeader") &&
                                     pageStack.__propagated.animateHeader

        // Call this function before pushing or popping to ensure correct order
        // of pushes/pops on the stack. This terminates any currently running
        // header transition.
        function finishPreviousAction() {
            // no action required when animating IN because the PageStack was
            // already updated before that transition started.
            if (internal.animateHeader && internal.headStyle.state == "OUT") {
                // force instant update of the PageStack without waiting for
                // the OUT animation to finish:
                internal.headStyle.animateOutFinished();
            }
        }

        // The PageWrapper to be pushed on the stack by pushWrapperObject().
        property var pageWrapper: null

        // Called when the header animate OUT transition finishes for push() or instantly
        // when header animations are disabled.
        function pushWrapperObject() {
            if (internal.animateHeader) {
                headStyle.animateOutFinished.disconnect(internal.pushWrapperObject);
            }
            if (internal.stack.size() > 0) internal.stack.top().active = false;
            internal.stack.push(internal.pageWrapper);
            internal.pageWrapper = null;
            internal.stackUpdated();
        }

        // Called when header animate OUT transition finishes for pop() or instantly
        // when header animations are disabled.
        function popAndDestroy() {
            if (internal.animateHeader) {
                headStyle.animateOutFinished.disconnect(internal.popAndDestroy);
            }
            internal.stack.top().active = false;
            if (internal.stack.top().canDestroy) internal.stack.top().destroyObject();
            internal.stack.pop();
            internal.stackUpdated();
        }

        /*!
          The instance of the stack from javascript
         */
        property var stack: new Stack.Stack()

        function createWrapper(page, properties) {
            var wrapperObject = pageWrapperComponent.createObject(pageStack);
            wrapperObject.pageStack = pageStack;
            wrapperObject.properties = properties;
            // set reference last because it will trigger creation of the object
            //  with specified properties.
            wrapperObject.reference = page;
            return wrapperObject;
        }

        // Update depth and makes the Item on top of the stack active, and
        // then animates IN the new header contents if header animations are enabled.
        function stackUpdated() {
            pageStack.depth = stack.size();
            if (pageStack.depth > 0) {
                internal.stack.top().active = true;
                currentPage = stack.top().object;

                if (internal.animateHeader) {
                    headStyle.animateIn();
                }
            } else {
                currentPage = null;
            }
        }
    }

    /*!
      \qmlproperty list<Object> data
      Children of PageStack are placed in a separate item such that they are
      not active by default until they are pushed on the PageStack.
     */
    default property alias data: inactiveNode.data
    PageTreeNode {
        anchors.fill: parent
        id: inactiveNode
        active: false
    }
}

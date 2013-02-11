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
    \qmltype ToolbarActions
    \inqmlmodule Ubuntu.Components 0.1
    \ingroup ubuntu
    \brief List of \l Action items with additional properties to control a toolbar.

    Examples: See \l Page.
*/
ActionList {
    id: toolbarActions

    /*!
      The back \l Action. If the action is visible, the back button will be shown
      on the left-side of the toolbar.
      If there is a \l PageStack with depth greater than 1, it will always be popped
      when this action is triggered. Override the back action to avoid that behavior.
     */
    property Action back: Action {
        iconSource: Qt.resolvedUrl("artwork/back.png")
        text: "Back"
        visible: (toolbarActions.__pageStack !== null) && toolbarActions.__pageStack.depth > 1
        /*!
          \internal
          FIXME: If this is not marked as internal, qdoc thinks it needs to be documented.
         */
        onTriggered: if (__pageStack && __pageStack.depth > 1) __pageStack.pop()
    }

    /*!
      \internal
      PageStack for the back button
     */
    // Cannot use PageStack here that will cause a loop in parsing the qml files
    property Item __pageStack: null

    /*!
      The toolbar is active
     */
    property bool active: false

    /*!
      The toolbar cannot be made active or inactive by bottom-edge swipes.
      If the ToolbarActions contains no visible actions, it is automatically
      locked (in inactive state).
     */
    property bool lock: !toolbarActions.hasVisibleActions()

    /*!
      Determine whether this ToolbarActions has any visible actions
     */
    function hasVisibleActions() {
        print("TBA checking!");
        print(back);
        print(back.visible);
        if (back && back.visible) return true;
        for (var i=0; i < __actionList.length; i++) {
            print("actionList["+i+"] = "+__actionList[i].visible);
            if (__actionList[i].visible) return true;
        }
        print("locking!");
        return false;

//        return ActionList.hasVisibleActions();
    }
}

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
// FIXME: When a module contains QML, C++ and JavaScript elements exported,
// we need to use named imports otherwise namespace collision is reported
// by the QML engine. As workaround, we use Theming named import.
// Bug to watch: https://bugreports.qt-project.org/browse/QTBUG-27645
import Ubuntu.Components 0.1 as Theming

/*!
    \qmltype ToolbarItems
    \inqmlmodule Ubuntu.Components 0.1
    \ingroup ubuntu
    \brief List of Items to be placed in a toolbar.childAt(

    Each \l Page has a tools property that can be set to change the tools of toolbar supplied
    by \l MainView when the \l Page is active. Each ToolbarItems consists of a set of
    Items and several properties that specify the behavior of the toolbar when the \l Page
    is active.

    When a \l Page is used inside a \l Tabs or \l PageStack, the toolbar will automatically show
    the tools of the active \l Page. When the active \l Page inside the \l Tabs or \l PageStack
    is updated by changing the selected \l Tab or by pushing/popping a \l Page on the \l PageStack,
    the toolbar will automatically hide, except if the new active \l Page has the \l locked property set.

    \l {http://design.ubuntu.com/apps/building-blocks/toolbar}{See also the Design Guidelines on Toolbars}.

    \qml
        import QtQuick 2.0
        import Ubuntu.Components 0.1

        MainView {
            width: units.gu(50)
            height: units.gu(50)

            Page {
                title: "Tools example"
                Label {
                    anchors.centerIn: parent
                    text: "Custom back button\nToolbar locked"
                }
                // TODO TIM: update docs
                tools: ToolbarActions {
                    Action {
                        text: "action 1"
                        iconSource: Qt.resolvedUrl("call_icon.png")
                    }
                    Action {
                        text: "action 2"
                        iconSource: Qt.resolvedUrl("call_icon.png")
                    }
                    back {
                        itemHint: Button {
                            id: cancelButton
                            text: "cancel"
                        }
                    }
                    locked: true
                    opened: true
                }
            }
        }
    \endqml
*/
Item {
    id: toolbarItems
    anchors.fill: parent

    /*!
      Default property, holds the content which will shown in the toolbar.
      \qmlproperty list<Object> contents
     */
    default property alias contents: toolsContainer.data

    /*!
      The back button. If it is visible, it button will be shown
      on the left-side of the toolbar.
      If there is a \l PageStack with depth greater than 1, the back button will be
      visible and triggering it will pop the page on top of the stack. If there is no
      \l PageStack with depth greater than 1, the back action is hidden by default
      (but the default setting can be changed by setting its visible property).
     */
    property Item back: ToolbarButton {
//        parent: backContainer
        iconSource: Qt.resolvedUrl("artwork/back.png")
        text: i18n.tr("Back")
        visible: true //toolbarItems.__pageStack && toolbarItems.__pageStack.depth > 1
        /*!
          \internal
          FIXME: If this is not marked as internal, qdoc thinks it needs to be documented.
         */
        onTriggered: if (toolbarItems.__pageStack && toolbarItems.__pageStack.depth > 1) toolbarItems.__pageStack.pop()
    }

    /*!
      \internal
      PageStack for the back button
     */
    // Cannot use PageStack here that will cause a loop in parsing the qml files
    property Item __pageStack: null

    /*!
      The toolbar is opened
     */
    property bool opened: false

    /*!
      The toolbar cannot be opened/closed by bottom-edge swipes.
      If the ToolbarActions contains no visible actions, it is automatically
      locked (in closed state).
     */
    property bool locked: !toolbarItems.__hasVisibleItems()

    /*!
      \internal
      Determine whether this ToolbarActions has any visible actions
     */
    function __hasVisibleItems() {
        // TODO
        return true;
    }

    Rectangle {
        color: "pink"
        id: backContainer
        anchors {
            left: parent.left
            top: parent.top
            bottom: parent.bottom
        }
        width: childrenRect.width

        // internal link to the previous back Item to unset its parent
        // when toolbarItems.back is updated.
        property Item previousBackItem: null

        function updateBackItem() {
            print("back changed from "+backContainer.previousBackItem+" to "+toolbarItems.back);
            if (backContainer.previousBackItem) backContainer.previousBackItem.parent = null;
            backContainer.previousBackItem = toolbarItems.back;
            if (toolbarItems.back) toolbarItems.back.parent = backContainer;
        }

        Connections {
            target: toolbarItems
            onBackChanged: backContainer.updateBackItem()
            Component.onCompleted: backContainer.updateBackItem()
        }
    }

    Row {
        id: toolsContainer
        anchors {
            right: parent.right
            bottom: parent.bottom
            top: parent.top
            rightMargin: units.gu(2)
        }
        spacing: units.gu(1)
    }
}

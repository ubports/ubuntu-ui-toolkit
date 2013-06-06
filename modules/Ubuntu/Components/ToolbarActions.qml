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

    Each \l Page has a tools property that can be set to change the tools of toolbar supplied
    by \l MainView when the \l Page is active. Each ToolbarActions consists of a set of
    \l Action items and several properties that specify the behavior of the toolbar when the \l Page
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
    id: toolbarActions
//    id: list
    // internal objects using nested elements,
    // which isn't allowed by QtObject; this fix makes this possible
    /*!
      Default property to allow adding of children. For example, the tools of a \l Page
      can be defined as follows:
      \qml
          import QtQuick 2.0
          import Ubuntu.Components 0.1

          MainView {
              width: units.gu(50)
              height: units.gu(80)

              Page {
                  title: "test page"

                  Label {
                      anchors.centerIn: parent
                      text: "Hello, world"
                  }

                  tools: ToolbarActions {
                      Action {
                          text: "action 1"
                      }
                      Action {
                          text: "action 2"
                      }
                  }
              }
          }
      \endqml
      where \l ToolbarActions is derived from ActionList.
      \qmlproperty list<Action> children
      */
    default property alias children: toolbarActions.actions

    /*!
      List of already defined actions when not defining them as children of the ActionList.
      Note that when you set this property, the children of the ActionList will be ignored,
      so do not set the list and define children.
      \qml
        import QtQuick 2.0
        import Ubuntu.Components 0.1

        MainView {
            width: units.gu(50)
            height: units.gu(80)

            Action {
                id: action1
                text: "action 1"
                onTriggered: print("one!")
            }
            Action {
                id: action2
                text: "action 2"
                onTriggered: print("two!")
            }

            Page {
                title: "test page"

                Label {
                    anchors.centerIn: parent
                    text: "Hello, world"
                }

                tools: ToolbarActions {
                    actions: [action1, action2]
                }
            }
        }
      \endqml
      the advantage of setting actions over using the children is that the same
      \l Action items can be used in several sets of actions.
      */
    property list<Action> actions

    /*!
      The back \l Action. If the action is visible, the back button will be shown
      on the left-side of the toolbar.
      If there is a \l PageStack with depth greater than 1, the back action will be
      visible and triggering it will pop the page on top of the stack. If there is no
      \l PageStack with depth greater than 1, the back action is hidden by default
      (but the default setting can be changed by setting its visible property).
     */
    property Action back: Action {
        iconSource: Qt.resolvedUrl("artwork/back.png")
        text: i18n.tr("Back")
        visible: toolbarActions.__pageStack && toolbarActions.__pageStack.depth > 1
        /*!
          \internal
          FIXME: If this is not marked as internal, qdoc thinks it needs to be documented.
         */
        onTriggered: if (toolbarActions.__pageStack && toolbarActions.__pageStack.depth > 1) toolbarActions.__pageStack.pop()
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
      \deprecated
      Use property opened instead.
     */
    property bool active
    /*!
      \deprecated
      \internal
     */
    onActiveChanged: {
        print("ToolbarActions.active property is DEPRECATED. Use opened instead.");
        toolbarActions.opened = active;
    }

    /*!
      \deprecated
      Use property locked instead.
     */
    property bool lock
    /*!
      \deprecated
      \internal
     */
    onLockChanged: {
        print("ToolbarActions.lock property is DEPRECATED. Use locked instead.");
        toolbarActions.locked = lock;
    }

    /*!
      The toolbar cannot be opened/closed by bottom-edge swipes.
      If the ToolbarActions contains no visible actions, it is automatically
      locked (in closed state).
     */
    property bool locked: !toolbarActions.__hasVisibleActions()

    /*!
      \internal
      Determine whether this ToolbarActions has any visible actions
     */
    function __hasVisibleActions() {
        if (back && back.visible) return true;
        for (var i=0; i < toolbarActions.actions.length; i++) {
            if (toolbarActions.actions[i].visible) return true;
        }
        return false;
    }
}

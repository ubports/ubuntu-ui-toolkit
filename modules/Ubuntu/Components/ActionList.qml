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
    \qmltype ActionList
    \inqmlmodule Ubuntu.Components 0.1
    \ingroup ubuntu
    \brief List of \l Action items
*/

QtObject {
    id: list
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
    default property alias children: list.actions

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
}

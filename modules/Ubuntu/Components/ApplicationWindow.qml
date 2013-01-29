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
    \qmltype ApplicationWindow
    \inqmlmodule Ubuntu.Components 0.1
    \ingroup ubuntu
    \brief The root Item for all applications

    \b{This component is under heavy development.}

    Examples: TODO
*/
Item {
    id: applicationWindow

    /*!
      \preliminary
      The list of actions that will be placed on the toolbar of the application
     */
//    property list<Action> tools
    property alias tools: toolbar.tools
    property alias back: toolbar.back

    default property alias contentsItem: contents.data
    Item {
        id: contents
        anchors.fill: parent
    }

    Toolbar {
        id: toolbar
//        property Item page: pageStack.currentPage
//        tools : page ? page.tools : null
        tools: [
            Action {
                text: "one"
                onTriggered: print("1")
            },
            Action {
                text: "two"
                onTriggered: print("2")
            }
        ]
        // FIXME: The toolbar back button will be made configurable from a Page property
//        back: Action {
//            iconSource: Qt.resolvedUrl("artwork/back.png")
//            text: "Back"
//            visible: pageStack.depth > 1
//            onTriggered: pageStack.pop()
//        }
    }
}

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
import "ListItems" as ListItem

/*!
    \qmlclass PageList
    \inqmlmodule Ubuntu.Components 0.1
    \brief Helper class that generates a set of \l Standard
        ListItems that link to the pages that were defined
        as children of this PageContainer.

    \b{This component is under heavy development.}

    Examples:
    \qml
    PageStack {
        id: pageStack
        anchors.fill: parent

        Page {
            id: firstPage
            title: "Colors"
            contents: PageList {
                anchors.fill: parent
                pageStack: pageStack

                Page {
                    title: "Red"
                    contents: Rectangle {
                        anchors.fill: parent
                        color: "red"
                    }
                }
                Page {
                    title: "Green"
                    contents: Rectangle {
                        anchors.fill: parent
                        color: "green"
                    }
                }
                Page {
                    title: "Blue"
                    contents: Rectangle {
                        anchors.fill: parent
                        color: "blue"
                    }
                }
            }
        }
    }
    \endqml
*/
PageContainer {
    id: pageList

    anchors {
        left: parent ? parent.left : undefined
        right: parent ? parent.right : undefined
    }

    /*!
      \preliminary
      The page stack to which pages will be pushed.
     */
    property PageStack pageStack

    height: childrenRect.height
    Column {
        id: container
        anchors {
            left: parent.left
            right: parent.right
        }
        height: childrenRect.height

        function pushToStack(page) {
            if (pageList.pageStack) pageList.pageStack.push(page);
        }

        Repeater {
            model: pageList.pages
            ListItem.Standard {
                text: modelData.title
                iconSource: modelData.iconSource
                progression: true
                onClicked: container.pushToStack(modelData)
            }
        }
    }
}

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
    \qmlclass Drilldown
    \inqmlmodule Ubuntu.Components 0.1
    \brief TODO

    \b{This component is under heavy development.}

    Examples: TODO
*/
Page {
    id: drilldown

    /*!
      \internal
      The \l PageStack to push and pop pages to.
     */
    property PageStack __pageStack

    default property alias children: pageContainer.children
    PageContainer {
        id: pageContainer
    }

    contents: Rectangle {
        width: 250
        color: "#eeeeee"
        height: Math.min(listItemContainer.height, parent.height)
//        anchors {
//            top: toolBar.bottom
//            left: parent.left
//            right: parent.right
//        }

        ListItem.Container {
            id: listItemContainer
            width: 250
            height: 300
            anchors.fill: parent
            Repeater {
                model: pageContainer.pages
                ListItem.Standard {
                    text: modelData.title
                    iconSource: modelData.iconSource
                    progression: true
                    onClicked: __pageStack.push(modelData)
                }
            }
        }
    }
}

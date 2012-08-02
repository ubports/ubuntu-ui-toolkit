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
PageContainer {
    id: drilldown

    /*!
      \internal
      The \l PageStack to push and pop pages to.
     */
    property Item __pageStack: null //parent ? parent.hasOwnProperty("__pageStack") ? parent.__pageStack : null : null

    function push(page) {
        //__pageStack.push(page);
        //if (parent) parent.push(page);
        print("parent: "+parent)
        if (drilldown.__pageStack) __pageStack.push(page)
        else if (parent) parent.push(page)
    }

//    function findPageStack() {
//        var item = drilldown.parent;
//        if (!item) return;
//        while (item.hasOwnProperty("__pageStack") && (item.__pageStack === undefined)) {
//            item = parent.parent;
//        }
//        if (item.hasOwnProperty("__pageStack")) drilldown.__pageStack = item.__pageStack;
//        else drilldown.__pageStack = item;
//    }

//    onParentChanged: findPageStack()
//    Component.onCompleted: findPageStack()

//    default property alias children: pageContainer.children
//    PageContainer {
//        id: pageContainer
//    }

    contents: Rectangle {
        width: 250
        color: "#eeeeee"
        height: Math.min(listItemContainer.height, parent.height)

        ListItem.Container {
            id: listItemContainer
            width: 250
            height: 300
            anchors.fill: parent
            Repeater {
                model: drilldown.pages
                ListItem.Standard {
                    id: listItem
                    text: modelData.title
                    iconSource: modelData.iconSource
                    progression: true
                    onClicked: drilldown.push(modelData)
                }
            }
        }
    }
}

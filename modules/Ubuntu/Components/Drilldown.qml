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

    function __push(page) {
        // Parent must be either a Drilldown or PageStack.
        if (!parent) return;
        if (parent.hasOwnProperty("stackSize")) parent.push(page);
        else parent.__push(page);
    }

    contents: ListItem.Container {
//        id: listItemContainer
        anchors.fill: parent
        Repeater {
            model: drilldown.pages
            ListItem.Standard {
//                id: listItem
                text: modelData.title
                iconSource: modelData.iconSource
                progression: true
//                onClicked: drilldown.__push(modelData)
            }
        }
    }
}

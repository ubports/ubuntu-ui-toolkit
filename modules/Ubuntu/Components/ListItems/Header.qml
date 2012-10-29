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
    \qmltype Header
    \inqmlmodule Ubuntu.Components.ListItems 0.1
    \ingroup ubuntu-listitems
    \brief Header for grouping list items together

    Examples:
    \qml
        import Ubuntu.Components.ListItems 0.1 as ListItem
        Column {
            ListItem.Header { text: "Title" }
            ListItem.Standard { text: "Item one" }
            ListItem.Standard { text: "Item two" }
            ListItem.Divider { }
            ListItem.Standard { text: "Item three" }
            ListItem.Standard { text: "Item four" }
        }
    \endqml
    \b{This component is under heavy development.}
*/

Image {
    height: units.gu(3)
    source: "artwork/ListItemDivider24px.png"
    anchors {
        left: parent ? parent.left : undefined
        right: parent ? parent.right : undefined
    }

    /*!
     \preliminary
     The text that is shown as the header text.
     \qmlproperty string text
    */
    property alias text: label.text

    LabelVisual {
        id: label
        font.italic: true
        fontSize: "medium"
        anchors {
            verticalCenter: parent.verticalCenter
            leftMargin: units.gu(1)
            left: parent.left
            right: parent.right
        }
    }
}

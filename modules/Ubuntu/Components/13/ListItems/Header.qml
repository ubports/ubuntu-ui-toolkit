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

import QtQuick 2.4

/*!
    \qmltype Header
    \inqmlmodule Ubuntu.Components.ListItems 1.0
    \ingroup ubuntu-listitems
    \brief Header for grouping list items together

    Examples:
    \qml
        import Ubuntu.Components.ListItems 1.3 as ListItem
        Column {
            ListItem.Header { text: "Title" }
            ListItem.Standard { text: "Item one" }
            ListItem.Standard { text: "Item two" }
            ListItem.Divider { }
            ListItem.Standard { text: "Item three" }
            ListItem.Standard { text: "Item four" }
        }
    \endqml
*/

Item {
    height: units.gu(4)
    anchors {
        left: parent ? parent.left : undefined
        right: parent ? parent.right : undefined
    }

    /*!
     The text that is shown as the header text.
     \qmlproperty string text
    */
    property alias text: label.text

    /*!
      \internal
      The \l foregroundGroundColor property stores the internal label color.
     */
    property alias __foregroundColor: label.color

    LabelVisual {
        id: label
        fontSize: "medium"
        font.weight: Text.Normal
        anchors {
            verticalCenter: parent.verticalCenter
            left: parent.left
            right: parent.right
            margins: units.gu(2)
        }
    }

    ThinDivider {
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
    }
}

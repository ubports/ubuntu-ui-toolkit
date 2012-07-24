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

/*!
    \qmlclass Header
    \inqmlmodule UbuntuUIToolkit 0.1
    \brief Header for grouping list items together

    Examples:
    \qml
        Container {
            Header { text: "Title" }
            Standard { text: "Item one" }
            Standard { text: "Item two" }
            Divider { }
            Standard { text: "Item three" }
            Standard { text: "Item four" }
        }
    \endqml
    \b{This component is under heavy development.}
*/
Base {
    id: listItemHeader
    height: image.implicitHeight

    /*!
      \preliminary
      The text that is shown as the header text.
      \qmlproperty string text
     */
    property alias text: label.text

    __isDivider: true

    Image {
        id: image
        source: "artwork/divider_24px.png"
        width: parent.width
    }

    LabelVisual {
        id: label
        font.italic: true
        fontSize: "medium"
        anchors {
            verticalCenter: parent.verticalCenter
            leftMargin: 10
            left: parent.left
            right: parent.right
        }
    }
}

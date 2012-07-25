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
import Ubuntu.Components 0.1

/*!
    \qmlclass Caption
    \inqmlmodule UbuntuUIToolkit 0.1
    \brief List item that shows a piece of text.

    Examples:
    \qml
        import Ubuntu.Components.ListItems as ListItem
        ListItem.Container {
            ListItem.Standard {
                text: "Default list item."
            }
            ListItem.Caption {
                text: "This is a caption text, which can span multiple lines."
            }
        }
    \endqml
    \b{This component is under heavy development.}
*/
Base {
    id: captionListItem
    height: framing.height + 10

    /*!
      \preliminary
      The text that is shown in the list item as a label.
      \qmlproperty string text
     */
    property alias text: captionText.text

    Rectangle {
        id: framing
        color: "#e0e0e0"
        radius: 5
        anchors {
            left: parent.left
            right: parent.right
            verticalCenter: parent.verticalCenter
            margins: 5
        }
        height: captionText.height + 10

        TextCustom {
            id: captionText
            anchors.centerIn: parent
            width: parent.width - 10
            wrapMode: Text.Wrap
            color: Qt.rgba(0.4, 0.4, 0.4, 1.0)
            horizontalAlignment: Text.AlignHCenter
        }
    }
}

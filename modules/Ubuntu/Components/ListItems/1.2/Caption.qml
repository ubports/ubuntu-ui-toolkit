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
import Ubuntu.Components 1.2

/*!
    \qmltype Caption
    \inqmlmodule Ubuntu.Components.ListItems 1.0
    \ingroup ubuntu-listitems
    \brief List item that shows a piece of text.
    \note \b{The component is deprecated. Use ListItem and Captiopns components instead.}

    Examples:
    \qml
        import Ubuntu.Components.ListItems 1.0 as ListItem
        Column {
            ListItem.Standard {
                text: "Default list item."
            }
            ListItem.Caption {
                text: "This is a caption text, which can span multiple lines."
            }
        }
    \endqml
*/
Item {
    height: captionText.contentHeight + __contentMargin * 2
    width: parent ? parent.width : units.gu(31)

    /*!
      The text that is shown in the list item as a label.
      \qmlproperty string text
     */
    property alias text: captionText.text

    /*! \internal
      The spacing inside the list item. This is already defined in ListItem.Empty but we can't access that from here.
     */
    property real __contentMargin: units.gu(2)

    Label {
        id: captionText
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.leftMargin: __contentMargin
        anchors.rightMargin: __contentMargin
        wrapMode: Text.Wrap
        color: Theme.palette.normal.backgroundText
        horizontalAlignment: Text.AlignLeft
        fontSize: "small"
    }
}

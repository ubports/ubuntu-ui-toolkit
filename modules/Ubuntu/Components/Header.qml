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
    \internal
    \qmltype Header
    \inqmlmodule Ubuntu.Components 0.1
    \ingroup ubuntu
    \brief Header used by \l PageStack
*/
Rectangle {
    id: header

    color: "#666666"
    height: 40

    /*!
      \preliminary
      Should a button to pop the current page from the stack be visible,
      if there is more than one page currently on the stack?
     */
    property alias showBackButton: backButton.visible

    /*!
      \preliminary
      The text shown as the title in the Header
     */
    property alias title: headerTitle.text

    /*!
      \preliminary
      The PageStack to pop() when the back button is clicked.
     */
    property Item pageStack

    AbstractButton {
        id: backButton
        anchors {
            left: parent.left
            top: parent.top
            bottom: parent.bottom
            margins: 2
        }
        width: visible ? backImage.width + 20 : 0
        visible: false
        onClicked: if (header.pageStack) pageStack.pop()

        Image {
            id: backImage
            anchors.centerIn: parent
            source: "ListItems/artwork/ListItemProgressionArrow.png"
            fillMode: Image.PreserveAspectFit
            rotation: 180
        }
    }

    TextCustom {
        id: headerTitle
        anchors {
            left: backButton.right
            top: parent.top
            bottom: parent.bottom
            right: parent.right
            margins: 5
        }
        color: "white"
        horizontalAlignment: Text.AlignLeft
        verticalAlignment: Text.AlignVCenter
    }
}

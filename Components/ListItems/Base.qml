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
import ".."

// parent class for all list items
Item {
    id: baseListItem
    width: 250

    signal clicked

    AbstractButton {
        anchors.fill: parent
        enabled: baseListItem.enabled
        onClicked: baseListItem.clicked()
    }

    property bool __showTopSeparator: false
    property bool __showBottomSeparator: false

    /*!
      \internal
      Override in divider classes that should never
      draw the one-pixel divider at the top or bottom
      of the list item.
     */
    property bool __isDivider: false

    Image {
        id: topSeparatorLine
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: visible ? 2 : 0
        source: "artwork/divider_Horizontal.png"
        visible: baseListItem.__showTopSeparator
    }
    Image {
        id: bottomSeparatorLine
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        height: visible ? 2 : 0
        source: "artwork/divider_Horizontal.png"
        visible: baseListItem.__showBottomSeparator
    }
}

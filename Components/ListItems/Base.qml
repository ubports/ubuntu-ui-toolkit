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

/*!
    \qmlclass Base
    \inqmlmodule UbuntuUIToolkit
    \brief The parent of all list items

    \b{This component is under heavy development.}

*/
Item {
    id: baseListItem
    width: 250

    signal clicked

    property bool topSeparator: false
    property bool bottomSeparator: true

    AbstractButton {
        anchors.fill: parent
        enabled: listItem.enabled
        // NOTE: If there is a control, and progression == false,
        // then the control must accept the clicked() signal.
        // TODO: Check whether clicked() is there and don't call it if not.
        //onClicked: (listItem.control && (listItem.progression == false)) ? listItem.control.clicked() : listItem.clicked()
        onClicked: baseListItem.clicked()
    }

    property real __topSeparatorHeight: topSeparatorLine.height
    property real __bottomSeparatorHeight: bottomSeparatorLine.height

    Image {
        id: topSeparatorLine
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: visible ? 2 : 0
        source: "artwork/divider_Vertical.png"
        visible: listItem.topSeparator
    }
    Image {
        id: bottomSeparatorLine
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        height: visible ? 2 : 0
        source: "artwork/divider_Vertical.png"
        visible: listItem.bottomSeparator
    }
}

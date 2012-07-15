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
    \qmlclass BackgroundVisual
    \inqmlmodule UbuntuUIToolkit
    \brief

    \b{This component is under heavy development.}
*/
Item {
    id: backgroundVisual

    property bool showTopSeparator: false
    property bool showBottomSeparator: true

    Image {
        id: topSeparatorLine
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: visible ? 2 : 0
        source: "artwork/divider_Vertical.png"
        visible: backgroundVisual.showTopSeparator
    }
    Image {
        id: bottomSeparatorLine
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        height: visible ? 2 : 0
        source: "artwork/divider_Vertical.png"
        visible: backgroundVisual.showBottomSeparator
    }

}

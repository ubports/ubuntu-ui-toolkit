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
import Ubuntu.Components 0.1

// frame
UbuntuShape {
    id: shape

    // style properties
    /*!
      Background fill color
      */
    property color backgroundColor
    property int backgroundColorAnimationDuration

    /*!
      Spacing between the frame and the text editor area
      */
    property var frameSpacing

    // visuals
    z: -1

    anchors.fill: parent
    color: backgroundColor

    MouseArea {
        anchors.fill: parent
        onPressed: if (!item.activeFocus && item.activeFocusOnPress) item.forceActiveFocus()
    }

    Binding {
        target: item.__internal
        property: "frameSpacing"
        value: shape.frameSpacing
    }
}

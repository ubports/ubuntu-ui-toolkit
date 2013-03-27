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
    // FIXME: needs type checking in themes to define the proper type to be used
    // if color type is used, alpha value gets lost
    property color backgroundColor: Qt.rgba(0, 0, 0, 0.1)
    property color errorColor: "red"
    property real backgroundOpacity

    /*!
      Spacing between the frame and the text editor area
      */
    property var frameSpacing
    property real overlaySpacing

    // visuals
    z: -1
    property bool error: (item.hasOwnProperty("errorHighlight") && item.errorHighlight && !item.acceptableInput)
    onErrorChanged: color = (error) ? errorColor : backgroundColor;
    color: backgroundColor
    anchors.fill: parent
    opacity: backgroundOpacity

    MouseArea {
        anchors.fill: parent
        onPressed: if (!item.activeFocus && item.activeFocusOnPress) item.forceActiveFocus()
    }

    Binding {
        target: item.__internal
        property: "frameSpacing"
        value: shape.frameSpacing
    }
    Binding {
        target: item.__internal
        property: "spacing"
        value: overlaySpacing
    }
}

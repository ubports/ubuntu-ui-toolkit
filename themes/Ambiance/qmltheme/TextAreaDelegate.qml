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
Item {
    id: visuals
    // style properties
    // FIXME: needs type checking in themes to define the proper type to be used
    // if color type is used, alpha value gets lost

    property color color
    /*!
      Background fill color
      */
    property color backgroundColor: Qt.rgba(0, 0, 0, 0.1)
    property color errorColor: "red"
    property real backgroundOpacity

    /*!
      Spacing between the frame and the text editor area
      */
    property var frameSpacing
    property real overlaySpacing

    anchors.fill: parent

    Binding {
        target: styledItem.__internal
        property: "frameSpacing"
        value: visuals.frameSpacing
    }
    Binding {
        target: styledItem.__internal
        property: "spacing"
        value: visuals.overlaySpacing
    }

    z: -1
    UbuntuShape {
        id: shape
        property bool error: (styledItem.hasOwnProperty("errorHighlight") && styledItem.errorHighlight && !styledItem.acceptableInput)
        onErrorChanged: (error) ? visuals.errorColor : visuals.backgroundColor;
        color: visuals.backgroundColor;
        anchors.fill: parent
        opacity: visuals.backgroundOpacity

        MouseArea {
            anchors.fill: parent
            onPressed: if (!styledItem.activeFocus && styledItem.activeFocusOnPress) styledItem.forceActiveFocus()
        }
    }
}

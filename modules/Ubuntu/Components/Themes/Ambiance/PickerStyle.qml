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
import Ubuntu.Components.ListItems 0.1

Item {
    id: control
    // style properties
    /*!
      Specifies the background color
      */
    property color backgroundColor: "#0A000000"
    /*!
      Background color for highlight.
      */
    property color highlightBackgroundColor: "#ffffffff"
    /*!
      Highlight color.
      */
    property color highlightColor: UbuntuColors.orange
    /*!
      Scale of the highlight item
      */
    property real highlightScaleFactor: 1.15
    /*!
      Thickness of the highlight component
      */
    property real highlightThickness: units.gu(5)

    /*!
      The content holder exposed to the Picker so tumbler list can be reparented to it.
      */
    property alias tumblerHolder: content

    anchors.fill: parent

    // frame
    UbuntuShape {
        anchors.fill: parent
        radius: "medium"
        color: Theme.palette.normal.overlay
        image: shapeSource
    }

    ShaderEffectSource {
        id: shapeSource
        sourceItem: background
        hideSource: true
        // FIXME: visible: false prevents rendering so make it a nearly
        // transparent 1x1 pixel instead
        opacity: 0.01
        width: 1
        height: 1
    }

    Rectangle {
        id: background
        anchors.fill: parent

        MouseArea {
            anchors.fill: parent
        }
        // background
        color: backgroundColor

        Item {
            id: content
            anchors.fill: parent
        }

        // highlight
        Rectangle {
            id: highlightItem
            y: (content.height - control.highlightThickness) / 2
            anchors {
                left: content.left
                right: content.right
            }
            height: control.highlightThickness
            color: highlightBackgroundColor
        }
        ThinDivider {
            anchors {
                left: content.left
                right: content.right
                bottom: highlightItem.top
            }
        }
        ThinDivider {
            anchors {
                left: content.left
                right: content.right
                top: highlightItem.bottom
            }
        }

        ShaderEffectSource {
            id: effectSource
            visible: false
            sourceItem: styledItem.itemList

            property real sourceRectMultiplier: 2.0
            // XXX: This works because the parent of magnifier is the same as sourceItem
            //  in this case. Otherwise coordinate transformations will be needed.
            sourceRect: Qt.rect(magnifier.x, magnifier.y, magnifier.width, magnifier.height)
            textureSize: Qt.size(magnifier.width*sourceRectMultiplier, magnifier.height*sourceRectMultiplier)
        }
        HighlightMagnifier {
            id: magnifier
            anchors.fill: highlightItem
            scaleFactor: control.highlightScaleFactor
            outputColor: control.highlightColor
            source: effectSource
        }
    }
}

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

        HighlightMagnifier {
            sourceItem: styledItem.itemList
            anchors.fill: highlightItem
            scaleFactor: control.highlightScaleFactor
            outputColor: control.highlightColor
        }

//        Loader {
//            id: highlightLoader
//            anchors.fill: highlightItem
//            asynchronous: false
//            sourceComponent: styledItem.circular ? wrapAround : linear
//            onStatusChanged: {
//                if (status === Loader.Ready && item) {
//                    item.delegate = (styledItem.hasOwnProperty("highlight") && styledItem.highlight) ?
//                                styledItem.highlight : styledItem.delegate;
//                    item.smooth = true;
//                    item.scale = control.highlightScaleFactor;
//                    item.enabled = false;
//                }
//            }

//            // control bingings for PathView
//            Binding {
//                target: highlightLoader.item
//                property: "offset"
//                value: loader.item.offset
//                when: loader.item && highlightLoader.item
//            }

//            // control bingings for ListView
//            Binding {
//                target: highlightLoader.item
//                property: "contentY"
//                value: loader.item.contentY - loader.item.originY
//                when: loader.item && highlightLoader.item
//            }
//        }
    }
}

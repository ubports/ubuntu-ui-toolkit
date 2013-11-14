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

PickerStyle {
    id: styleItem
//    frameWidth: 0
    backgroundColor: "#00000000"
    backgroundOpacity: 0.2
    overlay: false

    Item {
        id: hItem
        y: (styleItem.height - highlightThickness) / 2
        height: highlightThickness
        width: parent.width

        ThinDivider { id: topDivider; anchors.top: parent.top }
        ThinDivider { id: bottomDivider; anchors.top: parent.bottom }

        Rectangle {
            anchors {
                top: topDivider.bottom
                bottom: bottomDivider.top
                left: parent.left
                right: parent.right
            }
            color: "white"
        }
        // the PathView is not good, I cannot link with the underlaying List- or PathView so they
        // can be scrolled both the same time when either ot those is moved.
//        PathView {
//            id: pView
//            property Item tumbler: styledItem
//            anchors.fill: parent
//            clip: true

//            model: styledItem.model
//            delegate: styledItem.delegate
//            onCurrentItemChanged: currentItem.labelColor = "red"
//            // put the currentItem to the center of the view
//            preferredHighlightBegin: 0.5
//            preferredHighlightEnd: 0.5

//            pathItemCount: 2
//            snapMode: PathView.SnapToItem
//            flickDeceleration: 100

//            property int contentHeight: pathItemCount * highlightThickness
//            path: Path {
//                startX: pView.width / 2
//                startY: -(pView.contentHeight - pView.height) / 2
//                PathLine {
//                    x: pView.width / 2
//                    y: pView.height + (pView.contentHeight - pView.height) / 2
//                }
//            }
//        }

        Magnifier {
            anchors.fill: parent
            targetWidth: parent.width
            targetHeight: parent.height
            source: shaderSource
        }
        ShaderEffectSource {
            id: shaderSource
            anchors.fill: parent
            sourceItem: list ? list.currentItem : null
        }
    }

}

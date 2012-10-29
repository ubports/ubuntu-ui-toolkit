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

Item {
    id: foreground
    anchors.centerIn: parent

    property alias textColor: label.color

    // update control's implicitWidth
    Binding {
        target: item
        property: "implicitWidth"
        value: foreground.implicitWidth + 2*foreground.horizontalPadding
    }

    /*!
      \preliminary
      The padding on the left and right side of the button foreground.
    */
    property real horizontalPadding: units.gu(1)
    property real maximumWidth: item.width - 2*foreground.horizontalPadding
    property real spacing: (item.text == "" || item.iconSource == "") ? 0 : units.gu(1)
    property real verticalPadding: units.gu(0.5)

    width: icon.width + label.width + foreground.spacing
    height: item.height - 2*foreground.verticalPadding
    implicitWidth: icon.width + label.implicitWidth + foreground.spacing

    Image {
        id: icon
        source: item.iconSource
        fillMode: Image.PreserveAspectFit
        anchors.verticalCenter: foreground.verticalCenter
        opacity: item.enabled ? 1.0 : 0.5
    }

    TextCustom {
        id: label
        text: item.text
        anchors {
            verticalCenter: foreground.verticalCenter
            verticalCenterOffset: units.dp(-1)
        }
        fontSize: "medium"
        font.italic: true
        opacity: item.enabled ? 1.0 : 0.5
        elide: Text.ElideRight

        property real availableWidth: foreground.maximumWidth - icon.width - foreground.spacing
        width: (implicitWidth < availableWidth) ? implicitWidth : availableWidth
    }

    Item {
        id: positioner

        states: [
            State {
                name: "right"
                AnchorChanges {
                    target: icon;
                    anchors.right: foreground.right
                }
                AnchorChanges {
                    target: label;
                    anchors.left: foreground.left
                }
            },
            State {
                name: "left"
                AnchorChanges {
                    target: icon;
                    anchors.left: foreground.left
                }
                AnchorChanges {
                    target: label;
                    anchors.right: foreground.right
                }
            },
            State {
                name: "center"
                AnchorChanges {
                    target: icon;
                    anchors.horizontalCenter: foreground.horizontalCenter
                }
                AnchorChanges {
                    target: label;
                    anchors.horizontalCenter: foreground.horizontalCenter
                }
            }
        ]
        state: (item.iconSource == "" || item.text == "") ? "center" : iconPosition
    }
}

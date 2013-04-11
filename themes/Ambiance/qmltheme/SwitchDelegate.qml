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

Item {
    id: visuals
    // style properties
    property url crossSource
    property url checkMarkSource
    /*!
      The opacity of the "cross" image when the switch is not checked,
      and opacity of the "checkMark" image when the switch is checked.
     */
    property real selectedImageOpacity: 1.0
    /*!
      The opacity of the "cross" image when the switch is checked,
      and the opacity of the "checkMark" image when the switch is not checked.
     */
    property real unselectedImageOpacity: 1.0
    /*!
      Spacing around the thumb.
     */
    property real thumbSpacing: units.dp(1)
    /*!
      Color of the thumb when the switch is checked.
     */
    property color checkedThumbColor
    /*!
      Color of the thumb when the switch is not checked.
     */
    property color uncheckedThumbColor
    /*!
      The animation to fade the color from checkedColor to uncheckedColor and vice versa.
     */
    property ColorAnimation thumbColorAnimation: ColorAnimation{duration: 0}
    /*!
      The animation on x to move the thumb.
     */
    property NumberAnimation moveThumbAnimation: NumberAnimation{duration: 0}

    property color backgroundColor: Qt.rgba(0.5, 0.5, 0.5, 0.5)
    property real thumbWidth: units.gu(4)
    property real thumbHeight: units.gu(4)

    // code
    anchors.fill: parent
    implicitWidth: 2*thumb.width + 3*thumb.spacing
    implicitHeight: thumb.height + 2*thumb.spacing
    opacity: item.enabled ? 1.0 : 0.5

    Component.onCompleted: {
        item.implicitWidth = implicitWidth;
        item.implicitHeight = implicitHeight;
    }

    UbuntuShape {
        id: background
        anchors.fill: parent
        color: backgroundColor
        gradientColor: "transparent"

        UbuntuShape {
            id: thumb

            // Thumb spacing is used a lot, so import it here once
            property real spacing: visuals.thumbSpacing

            width: visuals.thumbHeight
            height: visuals.thumbWidth
            x: item.checked ? rightThumbPosition.x : leftThumbPosition.x
            y: leftThumbPosition.y

            color: item.checked ? visuals.checkedThumbColor
                                : visuals.uncheckedThumbColor
            gradientColor: "transparent"

            Behavior on x {
                NumberAnimation {
                    duration: visuals.moveThumbAnimation.duration
                    easing: visuals.moveThumbAnimation.easing
                }
            }
            Behavior on color {
                ColorAnimation {
                    duration: visuals.thumbColorAnimation.duration
                    easing: visuals.thumbColorAnimation.easing
                }
            }
        }

        Item {
            id: leftThumbPosition
            anchors {
                left: parent.left
                top: parent.top
                leftMargin: thumb.spacing
                topMargin: thumb.spacing
            }
            height: thumb.height
            width: thumb.width

            Image {
                anchors.centerIn: parent
                opacity: item.checked ? visuals.unselectedImageOpacity
                                      : visuals.selectedImageOpacity
                source: visuals.crossSource
            }
        }

        Item {
            id: rightThumbPosition
            anchors {
                right: parent.right
                top: parent.top
                rightMargin: thumb.spacing
                topMargin: thumb.spacing
            }
            height: thumb.height
            width: thumb.width

            Image {
                anchors.centerIn: parent
                opacity: item.checked ? visuals.selectedImageOpacity
                                      : visuals.unselectedImageOpacity
                source: visuals.checkMarkSource
            }
        }
    }
}

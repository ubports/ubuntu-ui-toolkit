/*
 * Copyright 2013 Canonical Ltd.
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

import QtQuick 2.4
import Ubuntu.Components 1.2

Item {
    id: checkBoxStyle

    /*!
      The padding between the background shape and the outside border of the checkbox.
     */
    property real backgroundPadding: units.gu(0.33)

    /*!
      The background color when the checkbox is not checked.
     */
    property color uncheckedBackgroundColor: Qt.rgba(Theme.palette.normal.foreground.r,
                                                     Theme.palette.normal.foreground.g,
                                                     Theme.palette.normal.foreground.b, 0.2)

    /*!
      The background color when the checkbox is checked.
     */
    property color checkedBackgroundColor: UbuntuColors.green

    /*!
      The image to show inside the checkbox when it is checked.
     */
    property url iconSource: "image://theme/tick"

    /*!
      The color of the icon.
     */
    property color iconColor: Theme.palette.normal.foregroundText

    /*!
      The padding between the icon and the border of the thumb.
     */
    property real iconPadding: backgroundPadding

    opacity: enabled ? 1.0 : 0.5

    implicitWidth: units.gu(3)
    implicitHeight: units.gu(3)

    UbuntuShape {
        id: background
        anchors {
            fill: parent
            margins: checkBoxStyle.backgroundPadding
        }
        property real iconSize: Math.min(width, height) - 2*checkBoxStyle.iconPadding

        Icon {
            color: checkBoxStyle.iconColor
            width: background.iconSize
            height: background.iconSize
            id: tick
            anchors.centerIn: parent
            smooth: true
            source: checkBoxStyle.iconSource
            visible: styledItem.checked || transitionToChecked.running || transitionToUnchecked.running
        }

        state: styledItem.checked ? "checked" : "unchecked"
        states: [
            State {
                name: "checked"
                PropertyChanges {
                    target: tick
                    anchors.verticalCenterOffset: 0
                }
                PropertyChanges {
                    target: background
                    color: checkBoxStyle.checkedBackgroundColor
                }
            },
            State {
                name: "unchecked"
                PropertyChanges {
                    target: tick
                    anchors.verticalCenterOffset: checkBoxStyle.height
                }
                PropertyChanges {
                    target: background
                    color: checkBoxStyle.uncheckedBackgroundColor
                }
            }
        ]

        transitions: [
            Transition {
                id: transitionToUnchecked
                to: "unchecked"
                ColorAnimation {
                    target: background
                    duration: UbuntuAnimation.FastDuration
                    easing: UbuntuAnimation.StandardEasingReverse
                }
                SequentialAnimation {
                    PropertyAction {
                        target: background
                        property: "clip"
                        value: true
                    }
                    NumberAnimation {
                        target: tick
                        property: "anchors.verticalCenterOffset"
                        duration: UbuntuAnimation.FastDuration
                        easing: UbuntuAnimation.StandardEasingReverse
                    }
                    PropertyAction {
                        target: background
                        property: "clip"
                        value: false
                    }
                }
            },
            Transition {
                id: transitionToChecked
                to: "checked"
                ColorAnimation {
                    target: background
                    duration: UbuntuAnimation.FastDuration
                    easing: UbuntuAnimation.StandardEasing
                }
                SequentialAnimation {
                    PropertyAction {
                        target: background
                        property: "clip"
                        value: true
                    }
                    NumberAnimation {
                        target: tick
                        property: "anchors.verticalCenterOffset"
                        duration: UbuntuAnimation.FastDuration
                        easing: UbuntuAnimation.StandardEasing
                    }
                    PropertyAction {
                        target: background
                        property: "clip"
                        value: false
                    }
                }
            }
        ]
    }
}

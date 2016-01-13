/*
 * Copyright 2015 Canonical Ltd.
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
import Ubuntu.Components 1.3

Item {
    id: checkBoxStyle

    /*!
      The padding between the background shape and the outside border of the checkbox.
     */
    property real backgroundPadding: 0

    /*!
      The background color when the checkbox is not checked.
     */
    property color uncheckedBackgroundColor: styledItem.enabled
                                                ? theme.palette.normal.foreground
                                                : theme.palette.inactive.foreground

    /*!
      The background color when the checkbox is checked.
     */
    property color checkedBackgroundColor: styledItem.enabled ? theme.palette.normal.positive : theme.palette.inactive.raised

    /*!
      The image to show inside the checkbox when it is checked.
     */
    property url iconSource: "image://theme/tick"

    /*!
      The color of the icon.
     */
    property color iconColor: styledItem.enabled ? theme.palette.normal.positiveText : theme.palette.inactive.raisedText

    /*!
      The padding between the icon and the border of the thumb.
     */
    property real iconPadding: units.gu(0.4)

    implicitWidth: units.gu(2)
    implicitHeight: units.gu(2)

    FocusShape {
    }

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
                    backgroundColor: checkBoxStyle.checkedBackgroundColor
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
                    backgroundColor: checkBoxStyle.uncheckedBackgroundColor
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

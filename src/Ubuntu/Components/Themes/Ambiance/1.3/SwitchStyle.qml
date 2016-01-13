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
    id: switchStyle

    /*!
      The padding between the thumb and the outside border of the switch.
     */
    property real thumbPadding: 0

    implicitWidth: units.gu(5)
    implicitHeight: units.gu(2.5)
    LayoutMirroring.enabled: false
    LayoutMirroring.childrenInherit: true

    /*!
      The background color of the switch.
     */
    property color backgroundColor: styledItem.enabled ? theme.palette.normal.base : theme.palette.inactive.base

    /*!
      The background color of the checked switch.
      */
    property color checkedBackgroundColor: styledItem.enabled ? theme.palette.normal.positive : theme.palette.inactive.base
    /*!
      The background color of the thumb.
     */
    property color thumbColor: styledItem.enabled ? theme.palette.normal.raised : theme.palette.inactive.raised

    FocusShape {
    }

    UbuntuShape {
        id: background
        anchors.fill: parent
        backgroundColor: switchStyle.backgroundColor
        clip: true

        UbuntuShape {
            id: thumb
            states: [
                State {
                    name: "checked"
                    when: styledItem.checked
                    AnchorChanges {
                        target: thumb
                        anchors.left: undefined
                        anchors.right: parent.right
                    }
                    PropertyChanges {
                        target: background
                        backgroundColor: switchStyle.checkedBackgroundColor
                    }
                }
            ]

            transitions: [
                // Avoid animations on width changes (during initialization)
                // by explicitly setting from and to for the Transitions.
                Transition {
                    from: "*"
                    to: "checked"
                    reversible: true
                    enabled: animated
                    AnchorAnimation {
                        targets: thumb
                        duration: UbuntuAnimation.FastDuration
                        easing: UbuntuAnimation.StandardEasing
                    }
                    ColorAnimation {
                        target: thumb
                        properties: "backgroundColor"
                        duration: UbuntuAnimation.FastDuration
                        easing: UbuntuAnimation.StandardEasing
                    }
                }
            ]

            backgroundColor: switchStyle.thumbColor
            width: (background.width - switchStyle.thumbPadding * 3.0) / 2.0
            anchors {
                top: parent.top
                bottom: parent.bottom
                left: parent.left
                topMargin: switchStyle.thumbPadding
                bottomMargin: switchStyle.thumbPadding
            }

        }
    }
}

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

import QtQuick 2.0
import Ubuntu.Components 1.1

Item {
    id: switchStyle

    property real thumbPadding: units.gu(0.33)

    /* FIXME: setting the width and height is required because in the case no width
       is set on the Switch, then even though the width is set eventually to
       implicitWidth, it still goes through the value 0.0 which triggers an
       undesired animation if the Switch is checked.

       Example, values of width at instantiation:
         width = 0.0 (before SwitchStyle is loaded)
         width = implicitWidth (after SwitchStyle is loaded)
    */
//    width: implicitWidth
//    height: implicitHeight
    implicitWidth: units.gu(6)
    implicitHeight: units.gu(3)
    opacity: styledItem.enabled ? 1.0 : 0.5
    LayoutMirroring.enabled: false
    LayoutMirroring.childrenInherit: true

    /*!
      The background color of the switch.
     */
    property color backgroundColor: Theme.palette.normal.base

    /*!
      The background color of the thumb when the switch is checked.
     */
    property color checkedThumbColor: UbuntuColors.green

    /*!
      The background color of the thumb when the switch is not checked.
     */
    property color uncheckedThumbColor: Qt.rgba(0, 0, 0, 0.2)

    /*!
      The foreground color of the icon that is currently selected.
     */
    property color selectedIconColor: Theme.palette.normal.foregroundText

   /*!
     The color of the icon that is not currently selected.
    */
    property color unselectedIconColor: Theme.palette.normal.backgroundText

    /*!
      The source of the selected icon when the switch is checked.
     */
    property url checkedIconSource: "image://theme/tick"

    /*!
      The source of the selected icon when the switch is not checked.
     */
    property url uncheckedIconSource: "image://theme/close"

    UbuntuShape {
        id: background
        anchors.fill: parent
        color: switchStyle.backgroundColor
        clip: true

        UbuntuShape {
            id: thumb

            width: (background.width - switchStyle.thumbPadding * 3.0) / 2.0
            x: styledItem.checked ? rightThumbPosition.x : leftThumbPosition.x

            anchors {
                top: parent.top
                bottom: parent.bottom
                topMargin: switchStyle.thumbPadding
                bottomMargin: switchStyle.thumbPadding
            }

            property real iconSize: Math.min(width - 2*switchStyle.thumbPadding,
                                             height - 2*switchStyle.thumbPadding)

            color: styledItem.checked ? switchStyle.checkedThumbColor
                                      : switchStyle.uncheckedThumbColor

            Behavior on x {
                UbuntuNumberAnimation {
                    duration: UbuntuAnimation.FastDuration
                    easing: UbuntuAnimation.StandardEasing
                }
            }
            Behavior on color {
                ColorAnimation {
                    duration: UbuntuAnimation.FastDuration
                    easing: UbuntuAnimation.StandardEasing
                }
            }

            PartialColorize {
                anchors {
                    verticalCenter: parent.verticalCenter
                    right: parent.left
                    rightMargin: (parent.width - thumb.iconSize)/2 + switchStyle.thumbPadding
                }
                rightColor: switchStyle.unselectedIconColor
                source: Image {
                    source: switchStyle.uncheckedIconSource
                    sourceSize {
                        width: thumb.iconSize
                        height: thumb.iconSize
                    }
                }
            }

            PartialColorize {
                anchors {
                    verticalCenter: parent.verticalCenter
                    left: parent.right
                    leftMargin: (parent.width - thumb.iconSize)/2 + switchStyle.thumbPadding
                }
                rightColor: switchStyle.unselectedIconColor
                source: Image {
                    source: switchStyle.checkedIconSource
                    sourceSize {
                        width: thumb.iconSize
                        height: thumb.iconSize
                    }
                }
            }
        }

        Item {
            id: leftThumbPosition
            anchors {
                left: parent.left
                top: parent.top
                leftMargin: switchStyle.thumbPadding
                topMargin: switchStyle.thumbPadding
            }
            height: thumb.height
            width: thumb.width

            PartialColorize {
                anchors.centerIn: parent
                source: Image {
                    source: switchStyle.uncheckedIconSource
                    sourceSize {
                        width: thumb.iconSize
                        height: thumb.iconSize
                    }
                }
                progress: MathUtils.clamp((thumb.x - parent.x - x) / width, 0.0, 1.0)
                leftColor: "transparent"
                rightColor: switchStyle.selectedIconColor
            }
        }

        Item {
            id: rightThumbPosition
            anchors {
                right: parent.right
                top: parent.top
                rightMargin: switchStyle.thumbPadding
                topMargin: switchStyle.thumbPadding
            }
            height: thumb.height
            width: thumb.width

            PartialColorize {
                anchors.centerIn: parent
                source: Image {
                    source: switchStyle.checkedIconSource
                    sourceSize {
                        width: thumb.iconSize
                        height: thumb.iconSize
                    }
                }
                progress: MathUtils.clamp((thumb.x + thumb.width - parent.x - x) / width, 0.0, 1.0)
                leftColor: switchStyle.selectedIconColor
                rightColor: "transparent"
            }
        }
    }
}

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
import Ubuntu.Components 0.1

Item {
    id: switchStyle

    property real thumbPadding: units.gu(0.5)

    /* FIXME: setting the width and height is required because in the case no width
       is set on the Switch, then even though the width is set eventually to
       implicitWidth, it still goes through the value 0.0 which triggers an
       undesired animation if the Switch is checked.

       Example, values of width at instantiation:
         width = 0.0 (before SwitchStyle is loaded)
         width = implicitWidth (after SwitchStyle is loaded)
    */
    width: implicitWidth
    height: implicitHeight
    implicitWidth: units.gu(10)
    implicitHeight: units.gu(5)
    opacity: styledItem.enabled ? 1.0 : 0.5

    UbuntuShape {
        id: background
        anchors.fill: parent
        color: Theme.palette.normal.base

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

            color: styledItem.checked ? Theme.palette.selected.foreground
                                      : Theme.palette.normal.foreground

            Behavior on x {
                UbuntuNumberAnimation {
                    duration: UbuntuAnimation.SnapDuration
                    easing: UbuntuAnimation.StandardEasing
                }
            }
            Behavior on color {
                ColorAnimation {
                    duration: UbuntuAnimation.SnapDuration
                    easing: UbuntuAnimation.StandardEasing
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

            PartialColorizeImage {
                anchors.centerIn: parent
                source: Image {
                    source: "artwork/cross.png"
                }
                keyColorOut: styledItem.checked ? Theme.palette.normal.backgroundText
                                                : Theme.palette.selected.foregroundText
                Behavior on keyColorOut {
                    ColorAnimation {
                        duration: UbuntuAnimation.SnapDuration
                        easing: UbuntuAnimation.StandardEasing
                    }
                }
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

            PartialColorizeImage {
                anchors.centerIn: parent
                source: Image {
                    source: "artwork/tick.png"
                }
                keyColorOut: styledItem.checked ? Theme.palette.selected.foregroundText
                                                : Theme.palette.normal.backgroundText
                Behavior on keyColorOut {
                    ColorAnimation {
                        duration: UbuntuAnimation.SnapDuration
                        easing: UbuntuAnimation.StandardEasing
                    }
                }
            }
        }
    }
}

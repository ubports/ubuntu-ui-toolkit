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
    // styling properties
    property color checkedColor
    property color uncheckedColor
    /*!
      The image to show inside the checkbox when it is checked.
     */
    property url checkMarkSource
    /*!
      The animation to fade the color from checkedColor to uncheckedColor and vice versa.
     */
    property PropertyAnimation backgroundColorAnimation: PropertyAnimation{duration: 0}

    // code
    anchors.fill: parent
    opacity: enabled ? 1.0 : 0.5

    UbuntuShape {
        anchors.fill: parent
        color: item.checked ? checkedColor : uncheckedColor
        Behavior on color {
            ColorAnimation {
                duration: backgroundColorAnimation.duration
                easing: backgroundColorAnimation.easing
            }
        }
    }

    Image {
        id: checkMark
        anchors.centerIn: parent
        smooth: true
        source: checkMarkSource
        opacity: item.checked ? 1.0 : 0.0
        Behavior on opacity {
            UbuntuNumberAnimation { duration: UbuntuAnimation.SnapDuration }
        }
    }
}

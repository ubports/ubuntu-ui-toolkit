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
    property color checkedColor: "#DD4814"
    property color uncheckedColor: Qt.rgba(0.0, 0.0, 0.0, 0.1)

    /*!
      The image to show inside the checkbox when it is checked.
     */
    property url checkMarkSource: "artwork/CheckMark.png"
    /*!
      The animation to fade the color from checkedColor to uncheckedColor and vice versa.
     */
    property PropertyAnimation backgroundColorAnimation: ColorAnimation { duration: UbuntuAnimation.SnapDuration; easing: UbuntuAnimation.StandardEasingOut }

    // code
    anchors.fill: parent
    opacity: enabled ? 1.0 : 0.5

    implicitWidth: units.gu(4)
    implicitHeight: units.gu(4)

    UbuntuShape {
        anchors.fill: parent
        color: styledItem.checked ? checkedColor : uncheckedColor
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
        opacity: styledItem.checked ? 1.0 : 0.0
        Behavior on opacity {
            UbuntuNumberAnimation { duration: UbuntuAnimation.SnapDuration }
        }
    }
}

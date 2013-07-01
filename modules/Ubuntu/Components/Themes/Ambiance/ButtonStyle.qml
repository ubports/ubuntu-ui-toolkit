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
 *
 * Author: Florian Boucault <florian.boucault@canonical.com>
 */

import QtQuick 2.0
import Ubuntu.Components 0.1

Item {
    id: buttonStyle

    property Button button: styledItem
    property real minimumWidth: units.gu(10)
    property real horizontalPadding: units.gu(1)

    width: button.width
    height: button.height
    implicitWidth: Math.max(minimumWidth, foreground.implicitWidth + 2*horizontalPadding)
    implicitHeight: units.gu(5)

    UbuntuShape {
        id: background
        anchors.fill: parent
        color: button.color
        borderSource: "radius_idle.sci"
        visible: color.a != 0.0
    }

    UbuntuShape {
        id: backgroundPressed
        anchors.fill: parent
        color: background.color
        borderSource: "radius_pressed.sci"
        opacity: button.pressed ? 1.0 : 0.0
        Behavior on opacity {
            NumberAnimation {
                duration: UbuntuAnimation.SnapDuration
                easing.type: Easing.Linear
            }
        }
        visible: background.visible
    }

    ButtonForeground {
        id: foreground
        width: parent.width - 2*horizontalPadding
        anchors.centerIn: parent
        text: button.text
        /* Pick either a clear or dark text color depending on the luminance of the
           background color to maintain good contrast (works in most cases)
        */
        textColor: ColorUtils.luminance(button.color) <= 0.85 ? "#F3F3E7" : "#888888"
        iconSource: button.iconSource
        iconPosition: button.iconPosition
        iconSize: units.gu(3)
        spacing: horizontalPadding
        transformOrigin: Item.Top
        scale: button.pressed ? 0.98 : 1.0
        Behavior on scale {
            NumberAnimation {
                duration: UbuntuAnimation.SnapDuration
                easing.type: Easing.Linear
            }
        }
    }
}

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
    id: checkBoxStyle

    /*!
      The image to show inside the checkbox when it is checked.
     */
    property url tickSource: "artwork/tick.png"

    opacity: enabled ? 1.0 : 0.5

    implicitWidth: units.gu(4.25)
    implicitHeight: units.gu(4)

    UbuntuShape {
        anchors.fill: parent
        color: styledItem.checked ? Theme.palette.selected.foreground : Theme.palette.normal.foreground
        Behavior on color {
            ColorAnimation {
                duration: UbuntuAnimation.SnapDuration
                easing.type: Easing.Linear
            }
        }
    }

    Image {
        id: tick
        anchors.centerIn: parent
        smooth: true
        source: tickSource
        opacity: styledItem.checked ? 1.0 : 0.0
        Behavior on opacity {
            NumberAnimation {
                duration: UbuntuAnimation.SnapDuration
                easing.type: Easing.Linear
            }
        }
    }
}

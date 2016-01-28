/*
 * Copyright 2016 Canonical Ltd.
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
    id: visuals
    // styling properties
    property int iconWidth: units.gu(2)
    property int iconHeight: units.gu(2)

    implicitWidth: label.paintedWidth
    anchors.fill: parent

    Item {
        anchors.centerIn: parent
        height: icon.height + label.height + label.anchors.topMargin

        // FIXME: is there a reason we are still using styledItem.iconSource instead of iconName
        // so we coudl use the Icon?
        Icon {
            id: icon
            anchors {
                top: parent.top
                horizontalCenter: parent.horizontalCenter
            }
            width: iconWidth
            height: iconWidth
            name: styledItem.iconName
            color: label.color
        }

        Label {
            id: label
            anchors {
                horizontalCenter: parent.horizontalCenter
                top: icon.bottom
                topMargin: units.gu(1)
            }
            width: paintedWidth
            text: styledItem.text
            textSize: Label.XSmall
            color: styledItem.enabled
                   ? theme.palette.normal.overlayText
                   : theme.palette.disabled.overlayText
        }
    }

    Component.onCompleted: styledItem.implicitWidth = implicitWidth
}

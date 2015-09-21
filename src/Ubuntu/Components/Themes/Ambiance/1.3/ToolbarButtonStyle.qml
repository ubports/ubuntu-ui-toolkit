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
        opacity: styledItem.enabled ? 1.0 : 0.3

        Image {
            id: icon
            anchors {
                top: parent.top
                horizontalCenter: parent.horizontalCenter
            }
            sourceSize.width: iconWidth
            sourceSize.height: iconWidth
            source: styledItem.iconSource
            smooth: true
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
        }
    }

    Component.onCompleted: styledItem.implicitWidth = implicitWidth
}

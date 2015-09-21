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

// TODO: Remove this component when PageHeadStyle uses ActionBar
AbstractButton {
    id: button

    property real iconWidth: units.gu(2.5)
    property real iconHeight: iconWidth

    width: visible ? units.gu(5) : 0
    height: parent ? parent.height : undefined

    property alias color: icon.color

    Rectangle {
        visible: button.pressed
        anchors.fill: parent
        color: theme.palette.selected.background
    }

    Icon {
        id: icon
        anchors {
            centerIn: parent
        }
        // prevent trying to render the icon with an invalid source
        // when the button is invisible by setting width and height to 0
        width: visible ? button.iconWidth : 0
        height: visible ? button.iconHeight : 0
        source: button.iconSource
        color: Qt.rgba(0, 0, 0, 0)
        opacity: button.enabled ? 1.0 : 0.3
    }

    Component {
        id: labelComponent
        Label {
            id: label
            objectName: button.objectName + "_label"
            color: button.color
            opacity: button.enabled ? 1.0 : 0.3
            text: button.text
            textSize: Label.XxSmall
        }
    }
    Loader {
        anchors {
            top: icon.bottom
            topMargin: units.gu(0.5)
            horizontalCenter: parent.horizontalCenter
        }
        sourceComponent: button.state === "IconAndLabel" ? labelComponent : null
    }
}

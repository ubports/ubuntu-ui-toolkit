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
    id: iconButtonStyle

    implicitWidth: visible ? units.gu(5) : 0
    implicitHeight: units.gu(5) //parent ? parent.height : undefined

    /*!
      \qmlproperty color foregroundColor
      The color of the icons.
     */
    property alias foregroundColor: icon.color

    /*!
      The background color of the button.
     */
    property color backgroundColor: styledItem.pressed ?
                                        theme.palette.selected.background :
                                        "transparent"

    property real iconWidth: units.gu(2.5)
    property real iconHeight: iconWidth

    Rectangle {
        color: iconButtonStyle.backgroundColor
        anchors.fill: parent
    }

    Icon {
        id: icon
        anchors.centerIn: parent
        // prevent trying to render the icon with an invalid source
        // when the button is invisible by setting width and height to 0
        width: visible ? iconButtonStyle.iconWidth : 0
        height: visible ? iconButtonStyle.iconHeight : 0
        source: styledItem.iconSource
        color: Qt.rgba(0, 0, 0, 0)
        opacity: styledItem.enabled ? 1.0 : 0.3
    }

    Component {
        id: labelComponent
        Label {
            objectName: styledItem.objectName + "_label"
            color: iconButtonStyle.foregroundColor
            opacity: styledItem.enabled ? 1.0 : 0.3
            text: styledItem.text
            fontSize: "xx-small"
        }
    }
    Loader {
        anchors {
            top: icon.bottom
            topMargin: units.gu(0.5)
            horizontalCenter: parent.horizontalCenter
        }
        sourceComponent: styledItem.state === "IconAndLabel" ? labelComponent : null
    }
}

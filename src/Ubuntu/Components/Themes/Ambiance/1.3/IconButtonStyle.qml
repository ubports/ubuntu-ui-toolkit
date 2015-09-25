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

    implicitWidth: units.gu(5)
    implicitHeight: units.gu(5)

    /*!
      The color of the icons.
     */
    property color foregroundColor: "#808080"

    /*!
      The background color of the button.
     */
    property color backgroundColor: styledItem.pressed ?
                                        theme.palette.selected.background :
                                        "transparent"

    property real iconWidth: units.gu(2)
    property real iconHeight: iconWidth

    /*!
      Show a text label under the icon.
     */
    property bool showLabel: false

    Rectangle {
        id: background
        color: iconButtonStyle.backgroundColor
        anchors.fill: parent
    }

    Icon {
        id: icon
        anchors.centerIn: parent
        width: iconButtonStyle.iconWidth
        height: iconButtonStyle.iconHeight
        source: styledItem.iconSource
        color: iconButtonStyle.foregroundColor
        opacity: styledItem.enabled ? 1.0 : 0.3
    }

    Component {
        id: labelComponent
        Label {
            objectName: styledItem.objectName + "_label"
            color: iconButtonStyle.foregroundColor
            opacity: styledItem.enabled ? 1.0 : 0.3
            text: styledItem.text
            textSize: Label.XxSmall
        }
    }
    Loader {
        anchors {
            top: icon.bottom
            topMargin: units.gu(0.5)
            horizontalCenter: parent.horizontalCenter
        }
        sourceComponent: iconButtonStyle.showLabel ? labelComponent : null
    }
}

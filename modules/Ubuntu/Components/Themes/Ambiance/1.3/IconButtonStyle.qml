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
    id: style

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
        color: style.backgroundColor
        anchors.fill: parent
    }

    Icon {
        id: icon
        anchors.centerIn: parent

        // prevent trying to render the icon with an invalid source
        // when the button is invisible by setting width and height to 0
        width: visible ? style.iconWidth : 0
        height: visible ? style.iconHeight : 0
        source: styledItem.iconSource
//        color: Qt.rgba(0, 0, 0, 0)
        opacity: styledItem.enabled ? 1.0 : 0.3
    }
}

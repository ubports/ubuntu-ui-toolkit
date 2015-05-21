/*
 * Copyright 2014 Canonical Ltd.
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

/*!
  This component is DEPRECATED and no longer used. The divider is now a simple line.
 */
Item {
    id: dividerStyle

    property color color: styledItem.backgroundColor

    // Do not make the following two colors part of the style API as they may be deprecated soon.
    property color topColor: Qt.darker(color, 1.1)
    property color bottomColor: Qt.lighter(color, 1.2)

    Rectangle {
        anchors.fill: parent
        color: dividerStyle.color

        gradient: Gradient {
            // top shadow
            GradientStop {
                position: 0.02
                color: dividerStyle.topColor
            }
            // middle (background)
            GradientStop {
                position: 0.05
                color: dividerStyle.color
            }
            GradientStop {
                position: 0.95
                color: dividerStyle.color
            }
            // bottom highlight
            GradientStop {
                position: 0.98
                color: dividerStyle.bottomColor
            }
        }
    }
}

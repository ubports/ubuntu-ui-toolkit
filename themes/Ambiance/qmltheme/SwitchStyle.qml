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

QtObject {
    property url crossSource
    property url checkMarkSource

    /*!
      The opacity of the "cross" image when the switch is not checked,
      and opacity of the "checkMark" image when the switch is checked.
     */
    property real selectedImageOpacity: 1.0

    /*!
      The opacity of the "cross" image when the switch is checked,
      and the opacity of the "checkMark" image when the switch is not checked.
     */
    property real unselectedImageOpacity: 1.0

    /*!
      Spacing around the thumb.
     */
    property real thumbSpacing

    /*!
      Color of the thumb when the switch is checked.
     */
    property color checkedThumbColor

    /*!
      Color of the thumb when the switch is not checked.
     */
    property color uncheckedThumbColor

    /*!
      The animation to fade the color from checkedColor to uncheckedColor and vice versa.
     */
    property ColorAnimation thumbColorAnimation

    /*!
      The animation on x to move the thumb.
     */
    property NumberAnimation moveThumbAnimation

    property color backgroundColor
    property real thumbWidth: units.gu(4)
    property real thumbHeight: units.gu(4)
}

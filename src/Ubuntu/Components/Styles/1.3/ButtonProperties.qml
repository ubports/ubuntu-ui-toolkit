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

/*!
    \qmltype ButtonProperties
    \inqmlmodule Ubuntu.Components.Styles 1.3
    \ingroup style-api
    \brief Color properties for buttons.

    Defines the colors of buttons.
  */
QtObject {
    /*!
      The foreground color of an unpressed enabled button.
     */
    property color foregroundColor

    /*!
      The foreground color of a disabled button.
     */
    property color disabledForegroundColor

    /*!
      The foreground color of a button when being pressed.
     */
    property color pressedForegroundColor

    /*!
      The background color of an unpressed enabled button.
     */
    property color backgroundColor

    /*!
      The background color of a disabled button.
     */
    property color disabledBackgroundColor

    /*!
      The background color of a button when being pressed.
     */
    property color pressedBackgroundColor
}

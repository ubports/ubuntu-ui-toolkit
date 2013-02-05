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

    /*!
        Radius of the text input frame drawn around the input.
      */
    property string radius

    /*!
      Background fill color
      */
    property color backgroundColor
    property int backgroundColorAnimationDuration

    /*!
      Text color
      */
    property color color

    /*!
      Selected text color
      */
    property color selectedTextColor

    /*!
      Selection highlight color
      */
    property color selectionColor

    /*!
      Text font
      */
    property font font

    /*!
      Spacing between overlaid elements (e.g. hint)
      */
    property var overlaySpacing

    /*!
      Spacing between the frame and the text editor area
      */
    property var frameSpacing
}

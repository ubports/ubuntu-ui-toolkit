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
      Cursor color
      */
    property color color

    /*!
      Cursor transparency, may be used to show text under the cursor for thick cursors.
      */
    property real opacity

    /*!
      Cursor width
      */
    property real width

    /*!
      Properties driving cursor blinking. If either of these values are 0, no
      blinking is provided.
      */
    property bool blinking
    property int blinkTimeoutShown
    property int blinkTimeoutHidden

    /*!
      Selection mode pin styles
      */
    property var pinSize
    property var pinSensingOffset
    property color pinColor
}

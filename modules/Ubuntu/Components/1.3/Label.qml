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

/*!
    \qmltype Label
    \inqmlmodule Ubuntu.Components 1.1
    \ingroup ubuntu
    \brief Text with Ubuntu styling.

    Example:
    \qml
    Rectangle {
        color: UbuntuColors.coolGrey
        width: units.gu(30)
        height: units.gu(30)

        Label {
            anchors.centerIn: parent
            text: "Hello, world!"
            fontSize: "large"
        }
    }
    \endqml
*/
Text {
    id: label

    /*!
      The size of the text. One of the following strings (from smallest to largest):
        \list
          \li "xx-small"
          \li "x-small"
          \li "small"
          \li "medium"
          \li "large"
          \li "x-large"
        \endlist
        Default value is "medium".
      */
    property string fontSize: "medium"

    font.pixelSize: FontUtils.sizeToPixels(fontSize)
    color: theme.palette.selected.backgroundText
}

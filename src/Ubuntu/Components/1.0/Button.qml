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
import Ubuntu.Components 1.0

/*!
   \internal
   Documentation in 11/Button.qml
*/
AbstractButton {
    id: button

    property color color: __styleInstance.defaultColor

    property Gradient gradient: __styleInstance.defaultGradient

    property font font: __styleInstance ? __styleInstance.defaultFont : Qt.font({family: "Ubuntu", pixelSize: FontUtils.sizeToPixels("medium")})

    property string iconPosition: "left"

    style: Theme.createStyleComponent("ButtonStyle.qml", button)
}

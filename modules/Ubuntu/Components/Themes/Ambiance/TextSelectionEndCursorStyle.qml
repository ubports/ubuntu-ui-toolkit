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

import QtQuick 2.2
import Ubuntu.Components 1.1

// FIXME : move the API into Ubuntu.Components.Style
Item {
    id: cursorStyle
    /*!
      Property specifying the visible timeout of the cursor. It is not mandatory
      for styles to define values for this property if the cursor blinking is not
      desired. A value of 0 turns off the cursor blinking.
      */
    property int cursorVisibleTimeout: 0
    /*!
      Property specifying the hidden timeout of the cursor. It is not mandatory
      for styles to define values for this property if the cursor blinking is not
      desired. A value of 0 turns off the cursor blinking.
      */
    property int cursorHiddenTimeout: 0

    /*!
      Component defining the default cursor visuals.
      */
    property Component cursorDelegate: delegate

    /*!
      The item pointing to the cursor handler. Styles should set to null if the
      cursor does not have handler at all.
      */
    property Item caret: caretItem

    // style body
    Component {
        id: delegate
        Rectangle {
            width: units.dp(1)
            // FIXME: Add a new color to the palette
            color: UbuntuColors.blue
        }
    }

    // caretItem
    Image {
        id: caretItem
        source: "artwork/caret_noshadow.png"
        anchors {
            top: parent.bottom
            horizontalCenter: parent.horizontalCenter
            topMargin: -units.gu(0.5)
            horizontalCenterOffset: LayoutMirroring.enabled ? -units.gu(0.7) : units.gu(0.7)
        }
    }
}

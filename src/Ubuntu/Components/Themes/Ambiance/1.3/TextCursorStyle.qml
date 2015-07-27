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

import QtQuick 2.4
import Ubuntu.Components 1.3

// FIXME : move the API into Ubuntu.Components.Style
Item {
    id: cursorStyle
    /*!
      Property specifying the visible timeout of the cursor. It is not mandatory
      for styles to define values for this property if the cursor blinking is not
      desired. A value of 0 turns off the cursor blinking.
      */
    property int cursorVisibleTimeout: 800
    /*!
      Property specifying the hidden timeout of the cursor. It is not mandatory
      for styles to define values for this property if the cursor blinking is not
      desired. A value of 0 turns off the cursor blinking.
      */
    property int cursorHiddenTimeout: 400

    /*!
      Component defining the default cursor visuals.
      */
    property Component cursorDelegate: delegate

    /*!
      The item pointing to the cursor handler. Styles should set to null if the
      cursor does not have handler at all.
      */
    property Item caret: caretItem

    /*!
      The width of the cursor.
      */
    property int cursorWidth: units.dp(2)

    // style body
    Component {
        id: delegate
        Rectangle {
            objectName: "text_cursor_style_" + styledItem.positionProperty
            width: cursorWidth
            // FIXME: Extend the palette and use palette values here
            color: UbuntuColors.blue
            visible: styledItem.positionProperty === "cursorPosition" && (blinkTimer.timerShowCursor || !blinkTimer.running)
            Timer {
                id: blinkTimer
                interval: cursorStyle.cursorVisibleTimeout
                running: (cursorStyle.cursorVisibleTimeout > 0) &&
                         (cursorStyle.cursorHiddenTimeout > 0) &&
                         styledItem.visible &&
                         shouldBlink
                repeat: true
                property bool shouldBlink: !styledItem.readOnly && !styledItem.contextMenuVisible
                property bool timerShowCursor: true
                onTriggered: {
                    interval = (interval == cursorStyle.cursorVisibleTimeout) ?
                                cursorStyle.cursorHiddenTimeout : cursorStyle.cursorVisibleTimeout;
                    timerShowCursor = !timerShowCursor;
                }
            }
        }
    }

    // caretItem
    Image {
        id: caretItem
        source: Qt.resolvedUrl("../artwork/caret_noshadow.png")
        objectName: "text_cursor_style_caret_" + styledItem.positionProperty
        anchors {
            top: parent.bottom
            horizontalCenter: parent.horizontalCenter
            horizontalCenterOffset: cursorWidth / 2
        }
    }
}

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
import Ubuntu.Components 1.0

// frame
// FIXME: stabilize API
Item {
    id: visuals
    // style properties
    property url iconSource: "artwork/clear.svg"
    // FIXME: needs type checking in themes to define the proper type to be used
    // if color type is used, alpha value gets lost

    property color color: (styledItem.focus || styledItem.highlighted) ? Theme.palette.selected.fieldText : Theme.palette.normal.fieldText
    /*!
      Background fill color
      */
    property color backgroundColor: (styledItem.focus || styledItem.highlighted) ? Theme.palette.selected.field : Theme.palette.normal.field
    property color errorColor: UbuntuColors.orange

    /*!
      Spacing between the frame and the text editor area
      */
    property real frameSpacing: units.gu(1)
    property real overlaySpacing: units.gu(0.5)

    /*!
      Property holding the timeout in milliseconds the component enters into selection mode.
      */
    property int selectionModeTimeout: 300

    /*!
      \qmlproperty Component defaultCursor.component
      \qmlproperty bool defaultCursor.blinking
      \qmlproperty int defaultCursor.cursorVisibleTimeout
      \qmlproperty int defaultCursor.cursorHiddenTimeout

      Defines the default cursor component and its blinking configuration. If either of the
      blinking properties are invalid (false, or zero) the cursor will not blink anymore.
      */
    property TextCursorStyle defaultCursor: TextCursorStyle {
        id: cursorStyle
        component: Rectangle {
            width: units.dp(1)
            color: Theme.palette.selected.foreground
            visible: blinkTimer.timerShowCursor
            Timer {
                id: blinkTimer
                interval: cursorStyle.cursorVisibleTimeout
                running: (cursorStyle.cursorVisibleTimeout > 0) &&
                         (cursorStyle.cursorHiddenTimeout > 0) &&
                         styledItem.cursorVisible
                repeat: true
                onRunningChanged: print("running", running)
                property bool timerShowCursor: true
                onTriggered: {
                    interval = (interval == cursorStyle.cursorVisibleTimeout) ?
                                cursorStyle.cursorHiddenTimeout : cursorStyle.cursorVisibleTimeout;
                    timerShowCursor = !timerShowCursor;
                }
            }
        }
        cursorVisibleTimeout: 800
        cursorHiddenTimeout: 400
    }

    /*!
      Component defining the default cursor caret visuals. The caret will be
      re-parented to the cursor item, therefore anchoring to the parent can
      be used to drive the positioning of the handler.
      */
    property Component defaultCaret: Image {
        source: "artwork/teardrop-left.png"
        anchors {
            top: parent.bottom
            horizontalCenter: parent.horizontalCenter
            topMargin: units.gu(-1)
            horizontalCenterOffset: units.gu(0.7)
        }
    }

    /*!
      Selection start caret.
      */

    /*!
      Selection end caret.
      */

    /*!
      Caret alignments
      */


    // style body
    anchors.fill: parent

    z: -1

    /*!
      Text input background
      */
    property Component background: UbuntuShape {
        property bool error: (styledItem.hasOwnProperty("errorHighlight") && styledItem.errorHighlight && !styledItem.acceptableInput)
        onErrorChanged: (error) ? visuals.errorColor : visuals.backgroundColor;
        color: visuals.backgroundColor;
        anchors.fill: parent
    }

    Loader {
        id: backgroundLoader
        sourceComponent: background
        anchors.fill: parent
    }
}

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
      This property drives the interactive behavior of the scrollbar
      */
    property bool interactive

    /*!
      Overlay: when set, opacity specified in overlayOpacity will be used;
      when no overlay is specuified, the scrollbar is 100% visible
      */
    property bool overlay
    property var overlayOpacityWhenHidden
    property var overlayOpacityWhenShown

    /*!
      Animation durations
      */
    property PropertyAnimation scrollbarFadeInAnimation
    property var scrollbarFadeOutPause
    property PropertyAnimation scrollbarFadeOutAnimation
    /*!
      Animations: fade out.
      */
    property PropertyAnimation sliderAnimation
    property PropertyAnimation thumbConnectorFading
    property PropertyAnimation thumbFading
    property color sliderColor
    property color thumbConnectorColor
    property url forwardThumbReleased
    property url forwardThumbPressed
    property url backwardThumbReleased
    property url backwardThumbPressed
    property real sensingAreaThickness
    property real scrollAreaThickness
    property real thumbConnectorMargin
}

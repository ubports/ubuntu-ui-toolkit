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
// FIXME: When a module contains QML, C++ and JavaScript elements exported,
// we need to use named imports otherwise namespace collision is reported
// by the QML engine. As workaround, we use Theming named import.
// Bug to watch: https://bugreports.qt-project.org/browse/QTBUG-27645
import Ubuntu.Components 0.1 as Theming
/*!
    \qmltype PopupBase
    \inqmlmodule Ubuntu.Components.Popups 0.1
    \ingroup ubuntu-popups
    \brief The base class for all dialogs, sheets and popovers. Do not use directly.

    Examples: See subclasses.
*/
Item {
    id: popupBase

    /*!
      The property holds the area used to dismiss the popups, the area from where
      mouse and touch events will be grabbed. By default this area is the application's
      main view.
    */
    property Item dismissArea: QuickUtils.rootObject

    /*!
      The property specifies whether to forward or not the mouse and touch events
      happening outside of the popover. By default all events are grabbed.
    */
    property bool grabDismissAreaEvents: true

    /*!
      The property specifies whether the popup is a modal item or not. The default
      is modal.
      */
    property bool modal: true

    anchors.fill: parent ? parent : undefined

    // without specifying width and height below, some width calculations go wrong in Sheet.
    // I guess popupBase.width is not correctly set initially
    width: parent ? parent.width : undefined
    height: parent ? parent.height : undefined

    /*!
      \preliminary
      Make the popup visible. Reparent to the background area object first if needed.
      Only use this function if you handle memory management. Otherwise use
      PopupUtils.open() to do it automatically.
    */
    function show() {
        if (!dismissArea)
            dismissArea = QuickUtils.rootObject

        // Without setting the parent, mapFromItem() breaks in internalPopupUtils.
        parent = dismissArea;
        popupBase.visible = true;
    }

    /*!
      \preliminary
      Hide the popup.
      Only use this function if you handle memory management. Otherwise use
      PopupUtils.close() to do it automatically.
    */
    function hide() {
        popupBase.visible = false;
    }

    /*!
        \internal
        When the popup is created by calling PopupUtils.open(),
        onVisibleChanged is connected to __closeIfHidden().
     */
    function __closeIfHidden() {
        if (!visible) PopupUtils.close(popupBase);
    }

    /*!
      \internal
      Foreground component excluded from IMA
      */
    property Item __foreground

    /*!
      \internal
      Set to true if the IMA should dismiss the area
      */
    property bool __closeOnDismissAreaPress: false

    // dimmer
    Rectangle {
        anchors.fill: parent
        color: Theming.ComponentUtils.style(popupBase, "dimColor", "black")
        opacity: Theming.ComponentUtils.style(popupBase, "dimOpacity", 0.6)
        visible: Theming.ComponentUtils.style(popupBase, "dim", false) && modal
    }

    Theming.InverseMouseArea {
        enabled: modal
        anchors.fill: __foreground
        sensingArea: dismissArea
        propagateComposedEvents: !grabDismissAreaEvents
        onPressed: if (__closeOnDismissAreaPress) popupBase.hide()
    }

    MouseArea {
        anchors.fill: __foreground
    }
}

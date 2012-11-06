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
//import "../mathUtils.js" as MathUtils
//import "popoverUtils.js" as PopoverUtils
import Ubuntu.Components 0.1

Item {
    id: popupBase
    anchors.fill: parent ? parent : undefined

    property Item caller;

    property real edgeMargins: units.gu(2)
    property real callerMargins: units.gu(0.5)

    function show() {
        if (parent !== QuickUtils.rootObject) parent = QuickUtils.rootObject;
        popupBase.visible = true;
    }

    function hide() {
        popupBase.visible = false;
    }

    // TODO: Destroy only from Utils.close()
    onVisibleChanged: if (!visible) popupBase.destroy()

//    QtObject {
//        id: internal

//        // FIXME: Do not assume always portrait orientation for smalls creens.
//        property bool smallScreen: false;

//        function detectScreen() {
//            if (Math.min(rootArea.width, rootArea.height) <= popover.requestedWidth + 2*theme.edgeMargins) {
//                smallScreen = true;
//            } else {
//                smallScreen = false;
//            }
//            popover.updatePosition();
//        }
//    }

//    onWidthChanged: internal.detectScreen()
//    onHeightChanged: internal.detectScreen()
}

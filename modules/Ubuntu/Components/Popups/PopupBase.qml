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

Item {
    id: popupBase

    anchors.fill: parent ? parent : undefined

    // without specifying width and height below, some width calculations go wrong in Sheet.
    // I guess popupBase.width is not correctly set initially
    width: parent ? parent.width : undefined
    height: parent ? parent.height : undefined

    function show() {
        if (parent !== QuickUtils.rootObject) {
            parent = QuickUtils.rootObject;
        }
        popupBase.visible = true;
    }

    function hide() {
        popupBase.visible = false;
    }

    // When the popup is created by calling PopupUtils.open(),
    //  onVisibleChanged is connected to __closeIfHidden().
    function __closeIfHidden() {
        if (!visible) PopupUtils.close(popupBase);
    }
}

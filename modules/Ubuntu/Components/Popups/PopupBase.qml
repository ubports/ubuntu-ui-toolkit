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
//import "popupUtils.js" as PopupUtils
import Ubuntu.Components 0.1

Item {
    id: popupBase

    anchors.fill: parent

    // without this, some width calculations go wrong in Sheet
    // I guess popupBase.width is not correctly set initially
    width: parent.width
    height: parent.height

    // TODO: make private?
    property Item caller;

    function show() {
        if (parent !== QuickUtils.rootObject) {
            parent = QuickUtils.rootObject;
        }
        popupBase.visible = true;
    }

    function hide() {
        popupBase.visible = false;
    }

    function __closeIfHidden() {
        if (!visible) {
            print("destroying!");
            //popupBase.parent = null;
            popupBase.destroy();
        }
    }

    // TODO: Destroy *only* from Utils.close()
    onVisibleChanged: {
        if (!visible) {
            popupBase.destroy();
        }
    }
}

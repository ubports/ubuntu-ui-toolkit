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
import "popoverUtils.js" as PopoverUtils
import Ubuntu.Components 0.1

Item {
    id: popupBase
    anchors.fill: parent ? parent : undefined

    property Item caller;

    // theme
    property real edgeMargins: units.gu(2)
    property real callerMargins: units.gu(0.5)

    function show() {
        if (parent !== QuickUtils.rootObject) parent = QuickUtils.rootObject;
        popupBase.visible = true;
    }

    function hide() {
        popupBase.visible = false;
    }

    // private
    function updatePosition(item) {
        var pos = new PopoverUtils.Positioning(item, popupBase, caller, edgeMargins, callerMargins);

        var minWidth = item.width + 2*edgeMargins;
        var minHeight = item.height + 2*edgeMargins;
        // TODO: do specialized positioning on small screens.

        var coords;
        if (!caller) {
            coords = pos.center();
        } else {
            coords = pos.auto();
        }

        item.x = coords.x;
        item.y = coords.y;
    }

    // TODO: Destroy *only* from Utils.close()
    onVisibleChanged: {
        if (!visible) {
            print("destroying");
            popupBase.destroy();
        }
    }
}

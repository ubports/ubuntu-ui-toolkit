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
import QtTest 1.0
import Ubuntu.Test 1.0

UbuntuTestCase {
    when: windowShown

    SignalSpy {
        id: signalSpy
    }

    function setupSpy(item, signalName) {
        if (item.hasOwnProperty("leadingActions")) {
            signalSpy.target = item;
            signalSpy.signalName = signalName;
        }
    }
    function spy(item, timeout) {
        if (timeout == undefined) {
            timeout = 500;
        }
        if (item.hasOwnProperty("leadingActions")) {
            signalSpy.wait(timeout);
            signalSpy.clear();
            signalSpy.target = null;
        } else {
            wait(timeout);
        }
    }

    function panelItem(item, leading) {
        return findInvisibleChild(item, (leading ? "ListItemPanelLeading" : "ListItemPanelTrailing"));
    }

    function rebound(item, watchTarget) {
        if (watchTarget === undefined) {
            watchTarget = item;
        }

        if (watchTarget.contentItem.x != watchTarget.contentItem.anchors.leftMargin) {
            mouseClick(item, 1, 1);
            tryCompareFunction(function() {
                return watchTarget.contentItem.x == watchTarget.contentItem.anchors.leftMargin;
            }, true, 500);
        }
    }

    // delayed swipe, gives few millisecond timeout between each move
    // so Repeater has time to create the panel actions in style
    function swipe(item, x, y, dx, dy) {
        setupSpy(item, "contentMovementEnded");
        flick(item, x, y, dx, dy, 0, 0, undefined, undefined, 100);
        spy(item);
    }
}

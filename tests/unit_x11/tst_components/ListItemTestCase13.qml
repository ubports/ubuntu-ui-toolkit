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
import Ubuntu.Components 1.3

/*
 * Common test case functions for ListItem. A CPO for unit tests.
 */
UbuntuTestCase {
    when: windowShown

    SignalSpy {
        id: signalSpy
    }

    // set up the spy component for an item and a signal
    function setupSpy(item, signalName) {
        if (item.hasOwnProperty("leadingActions")) {
            signalSpy.target = item;
            signalSpy.signalName = signalName;
            signalSpy.clear();
        }
    }
    // wait on the previosuly set up spy
    function spyWait(timeout) {
        if (timeout == undefined) {
            timeout = 500;
        }
        if (signalSpy.target) {
            signalSpy.wait(timeout);
            signalSpy.clear();
            signalSpy.target = null;
        } else {
            wait(timeout);
        }
    }

    // rebounds a ListItem
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
        spyWait();
    }
    function swipeNoWait(item, x, y, dx, dy) {
        flick(item, x, y, dx, dy, 0, 0, undefined, undefined, 100);
    }

    function tug(item, x, y, dx, dy) {
        setupSpy(item, "contentMovementEnded");
        TestExtras.touchDrag(0, item, Qt.point(x, y), Qt.point(dx, dy));
        spyWait();
    }

    // returns the leading or trailing panel item
    function panelItem(item, leading) {
        return findInvisibleChild(item, (leading ? "ListItemPanelLeading" : "ListItemPanelTrailing"));
    }

    function toggleSelectMode(view, enabled, scrollToTop) {
        if (view.hasOwnProperty("positionViewAtBeginning") && scrollToTop) {
            // use the topmost listItem to wait for rendering completion
            view.positionViewAtBeginning();
        }
        var listItem = findChild(view, "listItem0");
        verify(listItem);
        view.ViewItems.selectMode = enabled;
        // waitForRendering aint seems to be reliable here, so we wait ~400 msecs
        wait(400);
    }

    function toggleDragMode(view, enabled) {
        // use the topmost listItem to wait for rendering completion
        view.positionViewAtBeginning();
        var listItem = findChild(view, "listItem0");
        verify(listItem);
        view.ViewItems.dragMode = enabled;
        // waitForRendering aint seems to be reliable here, so we wait ~400 msecs
        wait(400);
    }

    function drag(view, from, to) {
        var dragArea = findChild(view, "drag_area");
        verify(dragArea, "Cannot locate drag area");

        // grab the source item
        view.positionViewAtBeginning(from,ListView.Beginning);
        var panel = findChild(view, "drag_panel" + from);
        verify(panel, "Cannot locate source panel");
        var dragPos = dragArea.mapFromItem(panel, centerOf(panel).x, centerOf(panel).y);
        // move the mouse
        var dy = Math.abs(to - from) * panel.height + units.gu(1);
        dy *= (to > from) ? 1 : -1;
        mousePress(dragArea, dragPos.x, dragPos.y);
        wait(100);
        var draggedItem = findChild(view.contentItem, "DraggedListItem");
        if (draggedItem) {
            setupSpy(draggedItem.__styleInstance.dropAnimation, "stopped");
        }
        // use 10 steps to be sure the move is properly detected by the drag area
        mouseMoveSlowly(dragArea, dragPos.x, dragPos.y, 0, dy, 10, 100);
        // drop it, needs two mouse releases, this generates the Drop event also
        mouseRelease(dragArea, dragPos.x, dragPos.y + dy);
        // needs one more mouse release
        mouseRelease(dragArea, dragPos.x, dragPos.y + dy);
        spyWait();
    }

    function expand(item, expand) {
        item.expansion.expanded = expand;
        wait(400);
    }
}

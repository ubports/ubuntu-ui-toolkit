/*
 * Copyright 2014 Canonical Ltd.
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
import QtTest 1.0
import Ubuntu.Test 1.0
import Ubuntu.Components 1.1
import Ubuntu.Components.ListItems 1.0

Item {
    width: units.gu(40)
    height: units.gu(60)

    ExpandablesColumn {
        id: expandablesColumn
        anchors { left: parent.left; top: parent.top; right: parent.right }
        height: units.gu(60)

        Expandable {
            id: expandable
            expandedHeight: contentColumn.height

            onClicked: expanded = !expanded

            Column {
                id: contentColumn
                anchors { left: parent.left; right: parent.right; top: parent.top }
                Rectangle {
                    anchors { left: parent.left; right: parent.right}
                    id: collapsedRect
                    color: "khaki"
                    height: expandable.collapsedHeight
                }
                Rectangle {
                    anchors { left: parent.left; right: parent.right }
                    height: units.gu(40)
                    color: "orange"
                }
            }
        }
    }

    UbuntuTestCase {
        name: "Expandable"
        when: windowShown

        function test_expanding_collapsing() {
            waitForRendering(expandable);
            // expand it and make sure it eventually reaches the expandedHeight
            expandable.expanded = true;
            tryCompare(expandable, "height", contentColumn.height);

            // Item's flickable must not be interactive when the full expandedHeight fits
            var flickable = findInvisibleChild(expandable, "__expandableContentFlickable");
            compare(flickable.interactive, false);

            // collapse it and make sure it eventually reaches the collapsedHeight
            expandable.expanded = false;
            tryCompare(expandable, "height", expandable.collapsedHeight);
        }

        function test_maxExpandingHeight() {
            // resize the column to something smaller
            expandablesColumn.height = units.gu(30);

            // Item's flickable must not be interactive when collapsed
            var flickable = findInvisibleChild(expandable, "__expandableContentFlickable");
            compare(flickable.interactive, false);

            expandable.expanded = true;
            tryCompare(expandable, "height", expandablesColumn.height - expandable.collapsedHeight);

            // Now that we're expanded and don't fit into the parent, flicking must be enabled
            compare(flickable.interactive, true);
        }

        function test_restoreCollapsingFlickable() {
            // resize the column to something smaller
            expandablesColumn.height = units.gu(30);

            expandable.expanded = true;
            tryCompare(expandable, "height", expandablesColumn.height - expandable.collapsedHeight);

            var flickable = findInvisibleChild(expandable, "__expandableContentFlickable");
            flickable.flick(0, -units.gu(500))
            tryCompare(flickable, "flicking", false);

            // contentY should be off the grid now
            verify(flickable.contentY != 0);

            // Now collapse and make sure that the contentY ends up again at 0
            expandable.expanded = false;
            tryCompare(expandable, "height", expandable.collapsedHeight);
            compare(flickable.contentY, 0);
        }

        function cleanup() {
            // Restore listview height
            expandablesColumn.height = units.gu(60);

            // restore collapsed state
            expandable.expanded = false;
            tryCompare(expandable, "height", expandable.collapsedHeight);
        }
    }
}

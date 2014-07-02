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
import Ubuntu.Components 1.0
import Ubuntu.Components.ListItems 1.0

Item {
    width: units.gu(40)
    height: units.gu(60)

    ListModel {
        id: dummyModel
        Component.onCompleted: {
            for (var i = 0; i < 20; ++i) {
                dummyModel.append({idx: i});
            }
        }
    }

    UbuntuListView {
        id: ubuntuListView
        anchors { left: parent.left; top: parent.top; right: parent.right }
        height: units.gu(20)
        clip: true
        model: dummyModel

        delegate: Expandable {
            id: expandable
            objectName: "expandable" + index
            expandedHeight: contentColumn.height

            onClicked: ubuntuListView.expandedIndex = index

            Column {
                id: contentColumn
                anchors { left: parent.left; right: parent.right; top: parent.top }
                Rectangle {
                    anchors { left: parent.left; right: parent.right}
                    id: collapsedRect
                    color: index % 2 == 0 ? "khaki" : "blue"
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
        name: "UbuntuListView"
        when: windowShown

        function initTestCase() {
            tryCompare(dummyModel, "count", 20);
        }

        function init() {
            waitForRendering(ubuntuListView);
        }

        function expandItem(item) {
            var index = ubuntuListView.indexAt(item.x, item.y);
            ubuntuListView.expandedIndex = index;
            var targetHeight = Math.min(item.expandedHeight, ubuntuListView.height - item.collapsedHeight);
            tryCompare(item, "height", targetHeight);
            waitForRendering(ubuntuListView)
        }

        function collapse() {
            if (ubuntuListView.expandedIndex == -1) {
                return;
            }

            var expandedItem = findChild(ubuntuListView, "expandable" + ubuntuListView.expandedIndex);
            ubuntuListView.expandedIndex = -1;
            tryCompare(expandedItem, "height", expandedItem.collapsedHeight);
            waitForRendering(ubuntuListView);
        }

        function test_expandedItem() {
            var item = findChild(ubuntuListView, "expandable1");
            expandItem(item);

            // expandedItem needs to point to the expanded item
            compare(ubuntuListView.expandedIndex, 1);
            // item must be expanded now
            compare(item.expanded, true);

            collapse();

            // expandedItem must be unset after collapsing
            compare(ubuntuListView.expandedIndex, -1);
        }

        function test_noScrollingNeeded() {
            var item = findChild(ubuntuListView, "expandable1");
            fuzzyCompare(ubuntuListView.mapFromItem(item).y, item.collapsedHeight, .5);

            expandItem(item);
            waitForRendering(ubuntuListView);

            fuzzyCompare(ubuntuListView.mapFromItem(item).y, item.collapsedHeight, .5);
        }

        function test_scrollToTop() {
            ubuntuListView.height = units.gu(30);
            ubuntuListView.positionViewAtIndex(0, ListView.Beginning)

            var item = findChild(ubuntuListView, "expandable1");
            fuzzyCompare(ubuntuListView.mapFromItem(item).y, item.collapsedHeight, 1);

            expandItem(item);

            fuzzyCompare(ubuntuListView.mapFromItem(item).y, 0, .5);
        }

        function test_scrollIntoView() {
            var item = findChild(ubuntuListView, "expandable9");
            expandItem(item);
            waitForRendering(ubuntuListView);

            // The item must be scrolled upwards, leaving space for one other item at the bottom
            fuzzyCompare(ubuntuListView.mapFromItem(item).y, ubuntuListView.height - item.collapsedHeight - item.expandedHeight, 1);
        }

        function test_collapseByClickingOutside() {
            // expand item 0
            var item = findChild(ubuntuListView, "expandable0");
            expandItem(item);

            // click on item 1
            var item1 = findChild(ubuntuListView, "expandable1");
            mouseClick(item1, item1.width / 2, item1.height / 2);

            // make sure item1 is collapsed
            tryCompare(item, "expanded", false);
        }

        function test_dimOthers() {
            var item = findChild(ubuntuListView, "expandable1");
            expandItem(item);

            for (var i = 0; i < ubuntuListView.contentItem.children.length; ++i) {
                var childItem = ubuntuListView.contentItem.children[i];
                if (childItem.hasOwnProperty("expanded")) {
                    compare(childItem.opacity, childItem.expanded ? 1 : .5)
                }

            }
        }

        function test_destroyAndRecreateExpanded() {
            var item = findChild(ubuntuListView, "expandable1");
            expandItem(item);

            // scroll the list to the bottom
            ubuntuListView.currentIndex = 0;
            ubuntuListView.positionViewAtIndex(ubuntuListView.count -1, ListView.End);

            // make sure the item is eventually destroyed
            tryCompareFunction(function() { return findChild(ubuntuListView, "expandable1") == null;}, true)

            // scroll the list back up
            ubuntuListView.positionViewAtIndex(0, ListView.Beginning)

            // wait until the item is recreated.
            tryCompareFunction(function() { return findChild(ubuntuListView, "expandable1") != null; }, true);
            item = findChild(ubuntuListView, "expandable1");
            compare(item.expanded, true);
            
        }

        function test_collapseOnClick() {
            var item = findChild(ubuntuListView, "expandable1");
            item.collapseOnClick = true;
            expandItem(item);

            compare(ubuntuListView.expandedIndex, 1);

            mouseClick(item, item.width / 2, item.collapsedHeight / 2);
            tryCompare(ubuntuListView, "expandedIndex", -1);

            // restore stuff we've changed
            item.collapseOnClick = false;
        }

        function cleanup() {
            // Restore listview height
            ubuntuListView.height = units.gu(60);
            collapse();
            // scroll the ListView back to top
            ubuntuListView.positionViewAtIndex(0, ListView.Beginning);
        }
    }
}

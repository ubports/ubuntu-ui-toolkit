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

import QtQuick 2.2
import QtTest 1.0
import Ubuntu.Test 1.0
import Ubuntu.Components 1.1
import Ubuntu.Components.ListItems 1.0

Item {
    width: units.gu(40)
    height: units.gu(60)

    ListModel {
        id: dummyModel
        Component.onCompleted: {
            reload();
        }
        function reload() {
            clear();
            for (var i = 0; i < 20; ++i) {
                dummyModel.append({idx: i});
            }
        }
    }

    // timer used to simulate the model refresh
    Timer {
        id: refreshTimer
        interval: 500
        onTriggered: {
            dummyModel.reload();
        }
    }

    UbuntuListView {
        id: ubuntuListView
        anchors.fill: parent
        clip: true
        model: dummyModel

        pullToRefresh {
            refreshing: refreshTimer.running
            onRefresh: refreshTimer.restart()
        }

        delegate: Standard {
            id: expandable
            text: "item " + index
        }
    }

    UbuntuTestCase {
        name: "UbuntuListView"
        when: windowShown

        SignalSpy {
            id: refreshSpy
            signalName: "onRefresh"
        }

        function initTestCase() {
            tryCompare(dummyModel, "count", 20);
        }

        function init() {
            waitForRendering(ubuntuListView, 1000);
        }

        function cleanup() {
            // scroll the ListView back to top
            ubuntuListView.positionViewAtIndex(0, ListView.Beginning);
            refreshSpy.clear();
            refreshSpy.target = null;
            ubuntuListView.pullToRefresh.enabled = false;
        }

        function test_0_defaults() {
            verify(ubuntuListView.hasOwnProperty("pullToRefresh"), "PullToRefresh is missing");
            compare(ubuntuListView.pullToRefresh.enabled, false, "PullToRefresh functionality is disabled by default");
        }

        function test_pullToRefresh_manual_refresh() {
            ubuntuListView.pullToRefresh.enabled = true;
            refreshSpy.target = ubuntuListView.pullToRefresh
            var x = ubuntuListView.width / 2;
            mouseDrag(ubuntuListView, x, units.gu(1), 0, ubuntuListView.height);
            refreshSpy.wait();
            tryCompareFunction(function() { return ubuntuListView.pullToRefresh.refreshing; }, false, 1000);
            waitForRendering(ubuntuListView, 1000);
        }
    }
}

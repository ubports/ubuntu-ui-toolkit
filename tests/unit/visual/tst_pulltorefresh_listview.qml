/*
 * Copyright 2016 Canonical Ltd.
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

Item {
    id: testMain
    width: units.gu(40)
    height: units.gu(71)

    ListView {
        id: listView
        anchors.fill: parent
        model: ListModel {
            Component.onCompleted: {
                for (var i = 0; i < 50; i++) {
                    append({text: "Item #" + i});
                }
            }
        }
        delegate: Label {
            height: units.gu(5)
            text: modelData
        }
        Timer {
            id: completeRefresh
            interval: 200
        }
        focus: true
        highlightFollowsCurrentItem: true
        highlight: Item {}

        PullToRefresh {
            id: refresh
            objectName: "pulltorefresh"
            onRefresh: completeRefresh.restart()
            refreshing: completeRefresh.running
        }
    }

    UbuntuTestCase {
        name: "PullToRefreshAPI"
        when: windowShown

        SignalSpy {
            id: stateSpy
            signalName: "onStateChanged"
            target: refresh.__styleInstance
        }

        SignalSpy {
            id: releaseToRefreshSpy
            signalName: "releaseToRefreshChanged"
            target: refresh
        }

        function init() {
            listView.positionViewAtIndex(0, ListView.Beginning)
        }
        function cleanup() {
            waitForRendering(refresh, 500);
            // wait till refresh returns to idle
            tryCompareFunction(function() {return refresh.__styleInstance.state}, "idle", 1000);
            stateSpy.clear();
            releaseToRefreshSpy.clear();
        }

        function test_0_api_defaults() {
            compare(refresh.target, listView, "The target must be the parent or sibling");
            compare(refresh.offset, 0, "The component should be at offset 0");
            compare(refresh.releaseToRefresh, false, "The component must be inactive");
            verify(refresh.content !== null, "Default content");
            compare(refresh.refreshing, false, "The component is not refreshing");
            verify(refresh.hasOwnProperty("refreshing"), "No refreshing signal defined");
            // style check
            verify(refresh.__styleInstance.hasOwnProperty("defaultContent"), "Wrong style used, has no defaultContent defined");
            verify(refresh.__styleInstance.hasOwnProperty("activationThreshold"), "Wrong style used, has no activationThreshold defined");
            verify(refresh.__styleInstance.hasOwnProperty("releaseToRefresh"), "Wrong style used, has no releaseToRefresh defined");
        }

        function test_pull_data() {
            var h = refresh.__styleInstance.activationThreshold;
            return [
                {tag: "no refresh", height: h, xfail: true},
                {tag: "refresh", height: 4 * h, xfail: false},
            ];
        }
        function test_pull(data) {
            var x = listView.width / 2;
            var y = units.gu(5);
            var height = data.height;
            flick(listView, x, y, 0, data.height);
            if (data.xfail) {
                expectFailContinue(data.tag, "No refresh should be triggered");
            }
            releaseToRefreshSpy.wait(500);
            if (data.xfail) {
                expectFailContinue(data.tag, "No refresh should be triggered");
            }
            compare(refresh.__styleInstance.state, "refreshing", "The refresh control is in " + refresh.__styleInstance.state + " state");
        }

        function test_automatic_refresh_animation() {
            completeRefresh.restart();
            stateSpy.wait(500);
            compare(refresh.__styleInstance.state, "refreshing", "Not refreshing!");
        }

        function test_no_refresh_animation_when_list_not_on_top() {
            // scroll down so the list will not be at top
            listView.positionViewAtIndex(10, ListView.Beginning)

            completeRefresh.restart();
            expectFailContinue("no refresh when not on top", "");
            stateSpy.wait(500);
            compare(refresh.__styleInstance.state, "", "Not refreshing!");
        }
    }
}

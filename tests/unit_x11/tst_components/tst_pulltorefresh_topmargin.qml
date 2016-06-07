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

import QtQuick 2.4
import Ubuntu.Components 1.3
import QtTest 1.0
import Ubuntu.Test 1.3

Item {
    width: units.gu(40)
    height: units.gu(71)
    id: root
    Flickable {
        id: flickable
        anchors.fill: parent
        contentHeight: 1.6*flickable.height
        Label {
            anchors.centerIn: parent
            text: "void"
        }
        PullToRefresh {
            id: pullToRefresh
            parent: flickable
            onRefresh: {
                refreshing = true;
                refreshing = false;
            }
        }
    }

    UbuntuTestCase {
        name: "PullToRefreshTopMargin"
        when: windowShown

        function test_reposition_after_refresh_bug1578619() {
            var initialTopMargin = flickable.topMargin;
            var initialContentY = flickable.contentY;
            flick(flickable, flickable.width/2, units.gu(10), 0, units.gu(40));
            tryCompare(flickable, "moving", false);
            tryCompare(flickable, "topMargin", initialTopMargin, 500,
                    "Initial topMargin of flickable is not restored after refreshing.");
            tryCompare(flickable, "contentY", initialContentY, 500,
                    "Initial contentY of flickable is not restored after refreshing.")
        }
    }
}

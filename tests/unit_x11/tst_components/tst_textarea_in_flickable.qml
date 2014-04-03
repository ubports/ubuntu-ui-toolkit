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
import QtTest 1.0
import Ubuntu.Test 0.1
import Ubuntu.Components 0.1

Item {
    id: root
    width: 200; height: 200

    property bool hasOSK: QuickUtils.inputMethodProvider !== ""

    Flickable {
        id: flickable
        anchors.fill: parent
        contentWidth: inFlickable.width
        contentHeight: inFlickable.height
        clip: true

        TextArea {
            id: inFlickable
            width: flickable.width
            autoSize: true
            maximumLineCount: 0
            text: "1\n2\n3\n4\n5\n6\n7\n8\n1\n1\n1\n1\n1\n1\n1\n1\n1\n1\n1\n1\n1\n1\n1\n1\n1\n1\n1"
        }
    }

    SignalSpy {
        id: flickableSpy
        target: flickable
        signalName: "movingChanged"
    }

    UbuntuTestCase {
        name: "TextAreaInFlickableAPI"
        when: windowShown
        function test_DoNotStealFlickEvents() {
            mouseClick(inFlickable, 10, 10);
            flick(inFlickable, Qt.point(50, 20), Qt.point(50, 0), 100);
            tryCompare(flickableSpy, "count", 2, 200);
        }
    }
}

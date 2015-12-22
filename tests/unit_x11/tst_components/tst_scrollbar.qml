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
 *
 * Author: Andrea Bernabei <andrea.bernabei@canonical.com>
 */

import QtQuick 2.4
import QtTest 1.0
import Ubuntu.Test 1.0
import Ubuntu.Components 1.3
import Ubuntu.Components.Styles 1.3
import QtQml.Models 2.1

Item {
    id: main
    width: units.gu(50)
    height: units.gu(100)


    Component {
        id: flickableComp
        Item {
            width: units.gu(20)
            height: units.gu(30)
            property alias flickable: freshFlickable
            property alias scrollbar: freshScrollbar
            property alias content: content
            Flickable {
                id: freshFlickable
                anchors.fill: parent
                contentHeight: content.height
                contentWidth: content.width
                clip: true
                Rectangle {
                    id: content
                    width: units.gu(40)
                    //make this much taller than the flickable to avoid timing issues when
                    //detecting if the flickable is flicking in the tests
                    height: units.gu(200)
                    color: "blue"
                }
            }
            Scrollbar {
                id: freshScrollbar
                flickableItem: parent.flickable
            }
        }
    }

    SignalSpy {
        id: signalSpy
    }
    SignalSpy {
        id: anotherSignalSpy
    }

    Flickable {
        anchors.fill: parent
        Column {
            id: column
            Repeater {
                model: layoutsModel
            }
        }
    }

    VisualItemModel {
        id: layoutsModel
        Flickable {
            id: flickable_bottomAlign_anchors
        }
        Scrollbar {
            id: scrollbar_bottomAlign_anchors
            flickableItem: flickable_bottomAlign_anchors
            align: Qt.AlignBottom
        }

        Flickable {
            id: flickable_topAlign_anchors
        }
        Scrollbar {
            id: scrollbar_topAlign_anchors
            flickableItem: flickable_topAlign_anchors
            align: Qt.AlignBottom
        }

        Flickable {
            id: flickable_leadingAlign_anchors
        }
        Scrollbar {
            id: scrollbar_leadingAlign_anchors
            flickableItem: flickable_leadingAlign_anchors
            align: Qt.AlignLeading
        }

        Flickable {
            id: flickable_trailingAlign_anchors
        }
        Scrollbar {
            id: scrollbar_trailingAlign_anchors
            flickableItem: flickable_trailingAlign_anchors
            align: Qt.AlignTrailing
        }
    }

    UbuntuTestCase {
        name: "Scrollbar"
        when: windowShown

        function getFreshFlickable() {
            var flickable = flickableComp.createObject(column)
            if (!flickable) {
                console.log("Error: dynamic item creation failed.")
            }
            return flickable
        }
        function setVeryLongContentItem(flickable) {
            flickable.contentHeight = flickable.height * 10 + 1
        }
        function setupSignalSpy(spy, target, signalName) {
            spy.clear()
            //reset signalName otherwise it will look for the old signalName in the new target
            spy.signalName = ""
            spy.target = target
            spy.signalName = signalName
        }

        function warningMsg(msg) {
            return testUtil.callerFile() + ": " + msg
        }

        function test_bottomAlign_anchors() {
            compare(scrollbar_bottomAlign_anchors.flickableItem,
                    flickable_bottomAlign_anchors, "wrong flickableItem")
            compare(scrollbar_bottomAlign_anchors.anchors.left,
                    scrollbar_bottomAlign_anchors.flickableItem.left, "left anchor")
            compare(scrollbar_bottomAlign_anchors.anchors.right,
                    scrollbar_bottomAlign_anchors.flickableItem.right, "right anchor")
            compare(scrollbar_bottomAlign_anchors.anchors.bottom,
                    scrollbar_bottomAlign_anchors.flickableItem.bottom, "bottom anchor")
            //we can't check that the top anchor is not anchored
        }
        function test_topAlign_anchors() {
            compare(scrollbar_topAlign_anchors.flickableItem,
                    flickable_topAlign_anchors, "wrong flickableItem")
            compare(scrollbar_topAlign_anchors.anchors.left,
                    scrollbar_topAlign_anchors.flickableItem.left, "left anchor")
            compare(scrollbar_topAlign_anchors.anchors.right,
                    scrollbar_topAlign_anchors.flickableItem.right, "right anchor")
            compare(scrollbar_topAlign_anchors.anchors.top,
                    scrollbar_topAlign_anchors.flickableItem.top, "top anchor")
        }
        function test_leadingAlign_anchors() {
            compare(scrollbar_leadingAlign_anchors.flickableItem,
                    flickable_leadingAlign_anchors, "wrong flickableItem")
            compare(scrollbar_leadingAlign_anchors.anchors.left,
                    scrollbar_leadingAlign_anchors.flickableItem.left, "left anchor")
            compare(scrollbar_leadingAlign_anchors.anchors.bottom,
                    scrollbar_leadingAlign_anchors.flickableItem.bottom, "bottom anchor")
            compare(scrollbar_leadingAlign_anchors.anchors.top,
                    scrollbar_leadingAlign_anchors.flickableItem.top, "top anchor")
        }
        function test_trailingAlign_anchors() {
            compare(scrollbar_trailingAlign_anchors.flickableItem,
                    flickable_trailingAlign_anchors, "wrong flickableItem")
            compare(scrollbar_trailingAlign_anchors.anchors.right,
                    scrollbar_trailingAlign_anchors.flickableItem.right, "right anchor")
            compare(scrollbar_trailingAlign_anchors.anchors.bottom,
                    scrollbar_trailingAlign_anchors.flickableItem.bottom, "bottom anchor")
            compare(scrollbar_trailingAlign_anchors.anchors.top,
                    scrollbar_trailingAlign_anchors.flickableItem.top, "top anchor")
        }

        function test_indicatorStyleWhileFlicking_shortContent() {
            var freshTestItem = getFreshFlickable()
            if (!freshTestItem) return

            var flickable = freshTestItem.flickable
            var scrollbar = freshTestItem.scrollbar

            compare(scrollbar.__styleInstance.veryLongContentItem, false, "Scrollable item should be short.")

            setupSignalSpy(signalSpy, flickable, "movingChanged")

            flick(flickable, 1, 2, units.gu(2), -units.gu(10))

            signalSpy.wait()
            compare(signalSpy.count, 1, "No movingChanged signal after simulating a flick.")
            compare(flickable.moving, true, "Flickable not moving after simulating a flick.")
            compare(scrollbar.__styleInstance.state, "indicator", "Wrong style while flicking.")

            //we don't set it up before because the hinting feature already changes the style to thumb
            //at the beginning
            setupSignalSpy(anotherSignalSpy, scrollbar.__styleInstance, "stateChanged")

            //make sure the flickable stops
            mousePress(flickable, 0, 0)
            mouseMove(flickable, 0, 0)
            mouseRelease(flickable, 0, 0)

            signalSpy.wait()
            compare(flickable.moving, false, "Flickable still moving after simulating mouse click.")
            compare(signalSpy.count, 2, "No movingChanged signal after Flickable stopped moving.")

            anotherSignalSpy.wait()
            compare(anotherSignalSpy.count, 1, "State unchanged after Flickable stopped moving.")
            compare(scrollbar.__styleInstance.state, "hidden", "Wrong style after the item stopped moving.")

            freshTestItem.destroy()
        }

        function test_thumbStyleWhileFlicking_veryLongContent() {
            var freshTestItem = getFreshFlickable()
            if (!freshTestItem) return

            var flickable = freshTestItem.flickable
            var scrollbar = freshTestItem.scrollbar

            setupSignalSpy(signalSpy, scrollbar.__styleInstance, "veryLongContentItemChanged")
            setVeryLongContentItem(flickable)
            signalSpy.wait()

            compare(scrollbar.__styleInstance.veryLongContentItem, true, "Very long content item not detected")

            setupSignalSpy(signalSpy, flickable, "movingChanged")
            flick(flickable, 1, 2, units.gu(2), -units.gu(10))

            signalSpy.wait()
            compare(signalSpy.count, 1, "No movingChanged signal after simulating a flick.")
            compare(flickable.moving, true, "Flickable not moving after simulating a flick.")
            compare(scrollbar.__styleInstance.state, "thumb", "Wrong style while flicking a very long item")

            //we don't set it up before because the hinting feature already changes the style to thumb
            //at the beginning
            setupSignalSpy(anotherSignalSpy, scrollbar.__styleInstance, "stateChanged")

            //make sure the flickable stops
            mousePress(flickable, 0, 0)
            mouseMove(flickable, 0, 0)
            mouseRelease(flickable, 0, 0)

            signalSpy.wait()
            compare(flickable.moving, false, "Flickable still moving after simulating mouse click.")
            compare(signalSpy.count, 2, "No movingChanged signal after Flickable stopped moving.")

            anotherSignalSpy.wait()
            compare(anotherSignalSpy.count, 1, "State unchanged after Flickable stopped moving.")
            compare(scrollbar.__styleInstance.state, "hidden", "Wrong style after a the item stopped moving.")
            freshTestItem.destroy()
        }
    }
}

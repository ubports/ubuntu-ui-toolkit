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

MainView {
    width: units.gu(40)
    height: units.gu(71)

    Column {
        anchors {
            fill: parent
            margins: units.gu(1)
        }
        UbuntuListView {
            id: listView
            width: parent.width
            height: units.gu(30)
            model: 10
            delegate: Standard {
                control: Slider {
                    objectName: "testSlider" + index
                }
            }
        }
        UbuntuListView {
            id: listView2
            width: parent.width
            height: units.gu(30)
            model: 10
            delegate: Slider {
                objectName: "testSlider" + index
            }
        }
    }

    UbuntuTestCase {
        name: "SliderAPI"
        when: windowShown

        SignalSpy {
            id: flickSpy
            signalName: "onMovementStarted"
        }

        function cleanup() {
            flickSpy.target = null;
            flickSpy.clear();
        }

        function test_slider_blocks_flickable_data() {
            return [
                {tag: "ListView with Slider in ListItem", flickable: listView},
                {tag: "ListView with Slider alone", flickable: listView2},
            ];
        }

        function test_slider_blocks_flickable(data) {
            flickSpy.target = data.flickable;
            var slider = findChild(data.flickable, "testSlider3");
            verify(slider, "cannot find test slider in " + data.tag);
            var sliderPos = slider.mapToItem(data.flickable, units.gu(10), 0);
            mouseDrag(data.flickable, sliderPos.x, sliderPos.y, units.gu(20), units.gu(20));
            waitForRendering(data.flickable, 200);
            compare(flickSpy.count, 0, "The Flickable should not move while Slider is active.");
        }
    }
}

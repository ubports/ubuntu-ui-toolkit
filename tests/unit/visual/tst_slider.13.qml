/*
 * Copyright 2014-2016 Canonical Ltd.
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
import Ubuntu.Test 1.3
import Ubuntu.Components 1.3
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
            height: units.gu(20)
            clip: true
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
            height: units.gu(20)
            clip: true
            model: 10
            delegate: Slider {
                objectName: "testSlider" + index
            }
        }
        Flickable {
            id: flickable
            width: parent.width
            height: units.gu(20)
            clip: true
            contentHeight: column.height
            Column {
                id: column
                width: parent.width
                height: childrenRect.height
                Repeater {
                    model: 10
                    Standard {
                        control: Slider {
                            objectName: "testSlider" + index
                        }
                    }
                }
            }
        }
        Flickable {
            id: flickable2
            width: parent.width
            height: units.gu(20)
            clip: true
            contentHeight: column2.height
            Column {
                id: column2
                width: parent.width
                height: childrenRect.height
                Repeater {
                    model: 10
                    Slider {
                        objectName: "testSlider" + index
                    }
                }
            }
        }
    }

    UbuntuTestCase {
        name: "SliderAPI"
        when: windowShown

        SignalSpy {
            id: valueSpy
            signalName: "onValueChanged"
        }
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
                {tag: "Flickable with Column of Slider in ListItem", flickable: flickable},
                {tag: "Flickable with Column of Slider alone", flickable: flickable2},
            ];
        }

        function test_slider_blocks_flickable(data) {
            flickSpy.target = data.flickable;
            var slider = findChild(data.flickable, "testSlider1");
            verify(slider, "cannot find test slider in " + data.tag);
            var sliderPos = slider.mapToItem(data.flickable, units.gu(10), 0);
            mouseDrag(data.flickable, sliderPos.x, sliderPos.y, units.gu(20), units.gu(20));
            waitForRendering(data.flickable, 200);
            compare(flickSpy.count, 0, "The Flickable should not move while Slider is active.");
            compare(data.flickable.interactive, true, "The Flickable aint got back to interactive mode.");
        }

        function test_arrows_move_slider_data() {
            return [
                {tag: "Left", key: Qt.Key_Left, to: -0.2, min: -10, max: 10},
                {tag: "Left from left third", key: Qt.Key_Left, from: -5, to: -5.2, min: -10, max: 10},
                {tag: "Left(live)", key: Qt.Key_Left, to: -0.2, min: -10, max: 10, live: true},
                {tag: "Right", key: Qt.Key_Right, to: 0.2, min: -10, max: 10},
                {tag: "Right from right third", key: Qt.Key_Right, from: 5, to: 5.2, min: -10, max: 10},
                {tag: "Right(live)", key: Qt.Key_Right, from: 0, to: 0.2, min: -10, max: 10, live: true},
                // No movement expected while holding modifiers
                {tag: "^Left", key: Qt.Key_Left, mod: Qt.ControlModifier, min: -10, max: 10},
                {tag: "^Right", key: Qt.Key_Right, mod: Qt.ControlModifier, min: -10, max: 10},
            ];
        }
        function test_arrows_move_slider(data) {
            var from = data.from ? data.from : 0;
            var to = data.to ? data.to : from;
            var slider = findChild(listView, "testSlider1");
            verify(slider, "cannot find test slider in " + data.tag);
            slider.minimumValue = data.min;
            slider.maximumValue = data.max;
            slider.value = from;

            slider.forceActiveFocus();
            compare(slider.activeFocus, true, "Slider didn't gain focus!");
            valueSpy.target = slider;
            valueSpy.clear();
            keyPress(data.key, data.mod);
            if (to != from)
                valueSpy.wait();
            compare(slider.value, to, "Slider has the wrong value!");
        }
    }
}

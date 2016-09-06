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
import QtTest 1.1
import Ubuntu.Test 1.3
import Ubuntu.Components 1.3
import Ubuntu.Components.Labs 1.0

Item {
    id: main
    width: units.gu(200)
    height: units.gu(70)

    Component {
        id: testHandle
        Rectangle {
            radius: units.gu(1)
            anchors {
                fill: handle
                leftMargin: units.dp(1)
                rightMargin: units.dp(1)
                topMargin: handle.height / 2 - units.gu(3)
                bottomMargin: handle.height / 2 - units.gu(3)
            }
            color: UbuntuColors.graphite
            scale: handle.containsMouse || handle.pressed ? 1.6 : 1.0
            Behavior on scale { UbuntuNumberAnimation {} }
        }
    }

    Component {
        id: testLayout
        SplitView {
            id: layout
            anchors.fill: parent
            property int when: 1
            handleDelegate: testHandle

            layouts: [
                SplitViewLayout {
                    id: mainLayout
                    when: layout.when == 1
                    ViewColumn {
                        preferredWidth: units.gu(10)
                        maximumWidth: units.gu(100)
                    }
                    ViewColumn {
                        fillWidth: true
                        minimumWidth: units.gu(10)
                        maximumWidth: units.gu(150)
                    }
                    ViewColumn {
                        preferredWidth: units.gu(15)
                    }
                    ViewColumn {
                        fillWidth: true
                        minimumWidth: units.gu(15)
                    }
                },
                SplitViewLayout {
                    when: layout.when == 2
                    ViewColumn {
                        preferredWidth: units.gu(30)
                        minimumWidth: units.gu(30)
                    }
                    ViewColumn {
                        fillWidth: true
                        minimumWidth: units.gu(40)
                    }
                }
            ]

            Rectangle {
                objectName: "column" + SplitView.column
                color: UbuntuColors.red
                height: parent.height
                width: units.gu(90)
                Label { text: parent.width }
            }
            Rectangle {
                objectName: "column" + SplitView.column
                color: UbuntuColors.green
                height: parent.height
                Label { text: parent.width }
            }
            Rectangle {
                objectName: "column" + SplitView.column
                color: UbuntuColors.blue
                height: parent.height
                Label { text: parent.width }
            }
            Rectangle {
                objectName: "column" + SplitView.column
                color: UbuntuColors.ash
                height: parent.height
                Label { text: parent.width }
            }
            Rectangle {
                objectName: "column" + SplitView.column
                color: "pink"
                height: parent.height
                width: units.gu(30)
                Label { text: parent.width }
            }
        }
    }

    Component {
        id: defaults
        SplitView {
        }
    }

    Sections {
        id: sections
        actions: [
            Action {
                text: "4 columns"
                onTriggered: testLoader.item.when = 1
            },
            Action {
                text: "2 columns"
                onTriggered: testLoader.item.when = 2
            }
        ]
    }

    Loader {
        id: testLoader
        anchors {
            fill: parent
            topMargin: sections.height
        }
        sourceComponent: testLayout
    }

    UbuntuTestCase {
        when: windowShown

        readonly property real defaultSpacing: units.dp(4)

        function loadTest(testCase) {
            testLoader.asynchronous = false;
            testLoader.sourceComponent = testCase;
            tryCompare(testLoader, "status", Loader.Ready)
            verify(testLoader.item);
            waitForRendering(testLoader.item);
            return testLoader.item;
        }

        SignalSpy {
            id: resizeSpy
            signalName: "preferredWidthChanged"
        }

        function cleanup() {
            testLoader.sourceComponent = null;
            resizeSpy.target = null;
            resizeSpy.clear();
        }

        function initTestCase() {
            // defaults
            var defs = loadTest(defaults);

            compare(defs.spacing, defaultSpacing);
            compare(defs.layouts.length, 0);
            compare(defs.activeLayout, null);
            compare(defs.handleDelegate, null);

            // cleanup is not called after initTestCase()
            testLoader.sourceComponent = null;
        }

        function test_visible_columns_data() {
            return [
                {tag: "4 columns from 5", when: 1, visible: [true, true, true, true, false]},
                {tag: "2 columns from 5", when: 2, visible: [true, true, false, false, false]},
                {tag: "0 columns from 5", when: 0, visible: [false, false, false, false, false]},
            ];
        }
        function test_visible_columns(data) {
            var test = loadTest(testLayout);
            test.when = data.when;
            for (var i in data.visible) {
                compare(findChild(test, "column" + i).visible, data.visible[i]);
            }
        }

        function test_column_width_data() {
            var fillWidth4 = (main.width - units.gu(10) - units.gu(15) - 3 * defaultSpacing) / 2;
            var fillWidth2 = main.width - units.gu(30) - defaultSpacing;
            return [
                {tag: "4 columns from 5", when: 1, width: [units.gu(10), fillWidth4, units.gu(15), fillWidth4, units.gu(30)]},
                // the columns from index 2 onwards use the 4-column configuration values
                {tag: "2 columns from 5", when: 2, width: [units.gu(30), fillWidth2, units.gu(15), fillWidth4, units.gu(30)]},
                {tag: "0 columns from 5", when: 2, width: [units.gu(30), fillWidth2, units.gu(15), fillWidth4, units.gu(30)]},
            ];
        }
        function test_column_width(data) {
            var test = loadTest(testLayout);
            test.when = data.when;
            for (var i in data.width) {
                compare(findChild(test, "column" + i).width, data.width[i], "column width at index " + i + " differs");
            }
        }

        function test_column_minmax_data() {
            var fillWidth4 = (main.width - units.gu(10) - units.gu(15)) / 2;
            var fillWidth2 = main.width - units.gu(30);
            return [
                {tag: "4 columns from 5", when: 1
                        , min: [0, units.gu(10), 0, units.gu(15), undefined]
                        , max: [units.gu(100), units.gu(150), Number.MAX_VALUE, Number.MAX_VALUE, undefined]
                },
                {tag: "2 columns from 5", when: 2
                        , min: [units.gu(30), units.gu(40), undefined, undefined, undefined]
                        , max: [Number.MAX_VALUE, Number.MAX_VALUE, undefined, undefined, undefined]
                },
                {tag: "0 columns from 5", when: 0
                        , min: [undefined, undefined, undefined, undefined, undefined]
                        , max: [undefined, undefined, undefined, undefined, undefined]
                },
            ];
        }
        function test_column_minmax(data) {
            var test = loadTest(testLayout);
            test.when = data.when;
            for (var i in data.min) {
                if (data.min[i] == undefined) {
                    // the attached config should be null
                    verify(!findChild(test, "column" + i).SplitView.columnConfig);
                } else {
                    compare(findChild(test, "column" + i).SplitView.columnConfig.minimumWidth, data.min[i], "column minimumWidth at index " + i + " differs");
                    compare(findChild(test, "column" + i).SplitView.columnConfig.maximumWidth, data.max[i], "column maximumWidth at index " + i + " differs");
                }
            }
        }

        // resize
        function test_resize_column() {
            var test = loadTest(testLayout);
            // move the mouse to the resizer handler
            var column0 = findChild(test, "column0");
            verify(column0);
            resizeSpy.target = column0.SplitView.columnConfig;
            // aim to the resize handle
            mouseDrag(column0, column0.width + test.spacing/2, column0.height / 2, units.gu(10), 0);
            resizeSpy.wait();
        }
        function test_resize_below_minimum() {
            var test = loadTest(testLayout);
            // move the mouse to the resizer handler
            var column1 = findChild(test, "column1");
            verify(column1);
            resizeSpy.target = column1.SplitView.columnConfig;
            // aim to the resize handle, and move large enough so the resize is long enough
            mouseDrag(column1, column1.width + test.spacing/2, column1.height / 2, -main.width, 0);
            resizeSpy.wait();
            compare(column1.width, column1.SplitView.columnConfig.minimumWidth);
        }

        // failure guards
        Component {
            id: badHandle
            QtObject {}
        }
        SplitView {
            id: bad
            layouts: SplitViewLayout {
                when: true
                ViewColumn {
                    preferredWidth: units.gu(10)
                }
                ViewColumn {
                    preferredWidth: units.gu(10)
                }
            }
            Rectangle {
                height: bad.height
            }
        }
        function test_invalid_handle() {
            ignoreWarning(warningFormat(282, 9, "QML SplitView: handle delegate not an Item"));
            bad.handleDelegate = badHandle;
        }

        function test_minimum_greater_than_maximum_value() {
            ignoreWarning(warningFormat(57, 21, "QML ViewColumn: minimumWidth is greater than maximumWidth"));
            var test = loadTest(testLayout);
            test.layouts[0].columns[0].minimumWidth = test.layouts[0].columns[0].maximumWidth + 1;
        }

        function test_invalid_minimum_value() {
            ignoreWarning(warningFormat(57, 21, "QML ViewColumn: minimumWidth cannot be a negative value"));
            var test = loadTest(testLayout);
            test.layouts[0].columns[0].minimumWidth = -1;
        }

        function test_maximum_smaller_than_minimum_value() {
            ignoreWarning(warningFormat(61, 21, "QML ViewColumn: maximumWidth is smaller than minimumWidth"));
            var test = loadTest(testLayout);
            test.layouts[0].columns[1].maximumWidth = test.layouts[0].columns[1].minimumWidth - 1;
        }

        function test_invalid_maximum_value() {
            ignoreWarning(warningFormat(61, 21, "QML ViewColumn: maximumWidth cannot be a negative value"));
            var test = loadTest(testLayout);
            test.layouts[0].columns[1].maximumWidth = -1;
        }

        function test_preferred_width_clamps_to_minimum() {
            var test = loadTest(testLayout);
            // turn fillWidth off
            test.layouts[0].columns[1].fillWidth = false;
            // minimum
            test.layouts[0].columns[1].preferredWidth = test.layouts[0].columns[1].minimumWidth - 1;
            compare(test.layouts[0].columns[1].preferredWidth, test.layouts[0].columns[1].minimumWidth);
        }

        function test_preferred_width_clamps_to_maximum() {
            var test = loadTest(testLayout);
            // turn fillWidth off
            test.layouts[0].columns[1].fillWidth = false;
            // clamps to max
            test.layouts[0].columns[1].preferredWidth = test.layouts[0].columns[1].maximumWidth + 1;
            compare(test.layouts[0].columns[1].preferredWidth, test.layouts[0].columns[1].maximumWidth);
        }

        function test_changing_minimum_clamps_preferredWidth() {
            var test = loadTest(testLayout);
            var config = test.layouts[0].columns[0];
            verify(config.preferredWidth < config.maximumWidth - 10);
            config.minimumWidth = config.maximumWidth - 10;
            compare(config.preferredWidth, config.minimumWidth);
        }

        function test_changing_maximum_clamps_preferredWidth() {
            var test = loadTest(testLayout);
            var config = test.layouts[0].columns[0];
            config.maximumWidth = config.preferredWidth - 10;
            compare(config.preferredWidth, config.maximumWidth);
        }

        // test layout change keeps value
        function test_resize_and_change_layouts() {
            var test = loadTest(testLayout);
            // move the mouse to the resizer handler
            var column0 = findChild(test, "column0");
            verify(column0);
            resizeSpy.target = column0.SplitView.columnConfig;
            // aim to the resize handle
            mouseDrag(column0, column0.width + test.spacing/2, column0.height / 2, units.gu(10), 0);
            resizeSpy.wait();

            var prevWidth = column0.width;

            // change layout
            test.when = 2;
            waitForRendering(test);
            verify(prevWidth != column0.width);

            // restore the previous layout
            test.when = 1;
            waitForRendering(test);
            compare(prevWidth, column0.width);
        }
    }
}

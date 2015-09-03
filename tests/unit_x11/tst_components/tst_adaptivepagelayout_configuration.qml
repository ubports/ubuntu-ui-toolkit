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
 */

import QtQuick 2.4
import QtTest 1.0
import Ubuntu.Test 1.0
import Ubuntu.Components 1.3
import QtQuick.Layouts 1.1

MainView {
    id: root
    width: units.gu(120)
    height: units.gu(71)

    property list<PageColumnsLayout> dynamicLayout: [
        PageColumnsLayout {
            when: layout.width >= units.gu(80)
            PageColumn {
                maximumWidth: units.gu(30)
                preferredWidth: units.gu(25)
            }
            PageColumn {
                fillWidth: true
            }
            PageColumn {
                preferredWidth: units.gu(20)
                fillWidth: true
            }
        },
        PageColumnsLayout {
            when: layout.width >= units.gu(30)
            PageColumn {
                maximumWidth: units.gu(30)
                preferredWidth: units.gu(25)
            }
            PageColumn {
                fillWidth: true
            }
        },
        PageColumnsLayout {
            when: true
            PageColumn {
                fillWidth: true
                minimumWidth: units.gu(20)
            }
        }
    ]
    property list<PageColumnsLayout> dynamicLayoutWrongOrder: [
        PageColumnsLayout {
            when: layout.width >= units.gu(30)
            PageColumn {
                maximumWidth: units.gu(30)
                preferredWidth: units.gu(25)
            }
            PageColumn {
                fillWidth: true
            }
        },
        PageColumnsLayout {
            when: layout.width >= units.gu(80)
            PageColumn {
                maximumWidth: units.gu(30)
                preferredWidth: units.gu(25)
            }
            PageColumn {
                fillWidth: true
            }
            PageColumn {
                preferredWidth: units.gu(20)
                fillWidth: true
            }
        },
        PageColumnsLayout {
            when: true
            PageColumn {
                fillWidth: true
                minimumWidth: units.gu(20)
            }
        }
    ]

    AdaptivePageLayout {
        id: layout
        width: parent.width
        height: parent.height

        primaryPage: page1
        layouts: PageColumnsLayout {
            id: defaultLayout
            when: layout.width >= units.gu(60)
            PageColumn {
                minimumWidth: units.gu(40)
                maximumWidth: units.gu(40)
            }
            PageColumn {
                fillWidth: true
            }
        }

        Page {
            id: page1
            title: "Page1"

            Column {
                anchors.centerIn: parent
                width: childrenRect.width
                Button {
                    text: "Page 2 left"
                    onTriggered: layout.addPageToCurrentColumn(page1, page2)
                }
                Button {
                    text: "Page 3 right"
                    onTriggered: layout.addPageToNextColumn(page1, page3);
                }
            }
        }
        Page {
            id: page2
            title: "Page2"
        }
        Page {
            id: page3
            title: "Page3"
        }
        Page {
            id: page4
            title: "Page4"
        }
    }

    UbuntuTestCase {
        when: windowShown

        function cleanup() {
            layout.layouts = defaultLayout;
            layout.width = root.width;
            waitForRendering(layout, 400);
        }

        function test_columns_data() {
            return [
                {tag: "one column", width: units.gu(59), columns: 1},
                {tag: "two columns", width: units.gu(60), columns: 2},
            ];
        }
        function test_columns(data) {
            layout.width = data.width;
            compare(layout.columns, data.columns, "the number of columns is wrong");
        }

        function test_update_layout_data() {
            return [
                {tag: "2 columns, proper order", width: units.gu(30), layout: dynamicLayout, columns: 2},
                {tag: "3 columns, proper order", width: units.gu(80), layout: dynamicLayout, columns: 3},
                {tag: "1 column, proper order", width: units.gu(29), layout: dynamicLayout, columns: 1},
                {tag: "2 columns, wrong order", width: units.gu(30), layout: dynamicLayoutWrongOrder, columns: 2},
                {tag: "2 columns, wrong order", width: units.gu(80), layout: dynamicLayoutWrongOrder, columns: 2},
                {tag: "1 column, wrong order", width: units.gu(29), layout: dynamicLayout, columns: 1},
            ];
        }
        function test_update_layout(data) {
            layout.layouts = data.layout;
            layout.width = data.width;
            waitForRendering(layout);
            compare(layout.columns, data.columns, "wrong columns in dynamic layout");
        }

        function test_sizes_data() {
            return [
                {tag: "1 column sizes, default layout", width: units.gu(50), layout: defaultLayout, columns: 1,
                            minimumWidths: [units.gu(40)], maximumWidths: [Number.POSITIVE_INFINITY], preferredWidths: [units.gu(40)], fillWidths: [true]},
                {tag: "2 column sizes, default layout", width: units.gu(60), layout: defaultLayout, columns: 2,
                            minimumWidths: [units.gu(40), 0], maximumWidths: [units.gu(40), Number.POSITIVE_INFINITY], preferredWidths: [units.gu(40), 0], fillWidths: [false, true]},
                {tag: "2 column sizes, custom layout", width: units.gu(30), layout: dynamicLayout, columns: 2,
                            minimumWidths: [0, 0], maximumWidths: [units.gu(30), Number.POSITIVE_INFINITY], preferredWidths: [units.gu(25), 0], fillWidths: [false, true]},
                {tag: "3 column sizes, custom layout", width: units.gu(80), layout: dynamicLayout, columns: 3,
                            minimumWidths: [0, 0, 0], maximumWidths: [units.gu(30), Number.POSITIVE_INFINITY, Number.POSITIVE_INFINITY], preferredWidths: [units.gu(25), 0, units.gu(20)], fillWidths: [false, true, true]},
                {tag: "1 column sizes, custom layout", width: units.gu(29), layout: dynamicLayout, columns: 1,
                            minimumWidths: [units.gu(20)], maximumWidths: [Number.POSITIVE_INFINITY], preferredWidths: [units.gu(20)], fillWidths: [true]},
            ];
        }
        function test_sizes(data) {
            layout.layouts = data.layout;
            layout.width = data.width;
            waitForRendering(layout);
            compare(layout.columns, data.columns, "columns mismatch");
            for (var i = 0; i < data.columns; i++) {
                var holder = findChild(layout, "ColumnHolder" + i);
                verify(holder, "Column #" + i + " not found");
                compare(holder.Layout.minimumWidth, data.minimumWidths[i], "minimum width mismatch for column #" + i);
                compare(holder.Layout.maximumWidth, data.maximumWidths[i], "maximum width mismatch for column #" + i);
                compare(holder.Layout.preferredWidth, data.preferredWidths[i], "preferred width mismatch for column #" + i);
                compare(holder.Layout.fillWidth, data.fillWidths[i], "fillWidth mismatch for column #" + i);
            }
        }

        function test_z_resize_data() {
            return [
                {tag: "not resizable", layout: defaultLayout, width: units.gu(60), move: units.gu(40), preResizeWidth: units.gu(40), postResizeWidth: units.gu(40)},
                {tag: "resizable to maximum", layout: dynamicLayout, width: units.gu(60), move: units.gu(40), preResizeWidth: units.gu(25), postResizeWidth: units.gu(30)},
            ];
        }
        function test_z_resize(data) {
            layout.layouts = data.layout;
            layout.width = data.width;
            waitForRendering(layout);
            // drag divider
            var holder = findChild(layout, "ColumnHolder0");
            verify(holder);
            var divider = findChild(holder, "Divider");
            verify(divider);
            mouseDrag(divider, centerOf(divider).x, centerOf(divider).y, data.move, 0);
            fuzzyCompare(holder.Layout.preferredWidth, data.postResizeWidth, 0.5, "resizing error");
        }
    }
}

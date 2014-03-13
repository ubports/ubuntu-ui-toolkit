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
import Ubuntu.Test 0.1
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1

Item {
    width: units.gu(40)
    height: units.gu(71)

    Column {
        ComboButton {
            id: combo
        }

        ComboButton {
            id: rectCombo

            Rectangle {
                id: rect
                width: parent.width
                height: units.gu(40)
                gradient: Gradient {
                    GradientStop {
                        position: 0.0
                        color: "red"
                    }
                    GradientStop {
                        position: 0.5
                        color: "green"
                    }
                    GradientStop {
                        position: 1.0
                        color: "blue"
                    }
                }
            }
        }

        ComboButton {
            id: columnCombo
            expandedHeight: -1
            Column {
                id: column
                anchors {
                    left: parent.left
                    right: parent.right
                }
                height: childrenRect.height
                Repeater {
                    model: 5
                    Rectangle {
                        width: parent.width
                        height: units.gu(5)
                        color: Qt.rgba(Math.random(1), Math.random(1), Math.random(1), 1);
                    }
                }
            }
        }

        ComboButton {
            id: longCombo
            expandedHeight: units.gu(40)
            Rectangle {
                id: longRect
                height: units.gu(10)
                color: "cyan"
            }
        }

        ComboButton {
            id: listCombo
            ListView {
                id: list
                model: 20
                height: listCombo.comboListHeight
                delegate: Standard {
                    text: modelData
                }
            }
        }
    }

    SignalSpy { id: spy }

    UbuntuTestCase {
        name: "ComboButton"
        when: windowShown

        function cleanup() {
            rectCombo.expanded = false;
            columnCombo.expanded = false;
            spy.clear();
            spy.signalName = "";
            spy.target = undefined;
        }

        function test_0_defaults() {
            compare(combo.expanded, false, "not expanded by default");
            compare(combo.autoCollapse, true, "automatically collapses");
            compare(combo.collapsedHeight, combo.implicitHeight, "collapsedHeight is implicitHeight");
            compare(combo.expandedHeight, combo.collapsedHeight + units.gu(15), "expanded height default");
            var lheight = combo.expandedHeight - combo.collapsedHeight - combo.__styleInstance.comboListMargin;
            compare(combo.comboListHeight, lheight, "comboListHeight default");
            compare(combo.comboList.length, 0, "comboList is empty");
        }

        function test_comboListWidth() {
            var comboListHolder = findChild(rectCombo, "combobutton_combolist");
            compare(rect.width, comboListHolder.width, "rectCombo list content width is not adjusted");
            comboListHolder = findChild(columnCombo, "combobutton_combolist");
            compare(column.width, comboListHolder.width, "columnCombo list content width is not adjusted");
            comboListHolder = findChild(longCombo, "combobutton_combolist");
            compare(longRect.width, comboListHolder.width, "longCombo list content width is not adjusted");
            comboListHolder = findChild(listCombo, "combobutton_combolist");
            compare(list.width, comboListHolder.width, "listCombo list content width is not adjusted");
        }

        function test_expandRectComboThroughProperty() {
            rectCombo.expanded = true;
            waitForRendering(rectCombo);
            var comboListPanel = findChild(rectCombo, "combobutton_combopanel");
            tryCompareFunction(function() { return comboListPanel.opacity}, 1.0);
        }

        function test_expandRectComboThroughClick() {
            var dropDown = findChild(rectCombo, "combobutton_dropdown");
            mouseClick(dropDown, dropDown.width / 2, dropDown.height / 2);
            waitForRendering(rectCombo);
            compare(rectCombo.expanded, true, "combo is not expanded");
            var comboListPanel = findChild(rectCombo, "combobutton_combopanel");
            tryCompareFunction(function() { return comboListPanel.opacity}, 1.0);
        }

        function test_autoCollapse() {
            var dropDown = findChild(rectCombo, "combobutton_dropdown");
            var comboListPanel = findChild(rectCombo, "combobutton_combopanel");
            mouseClick(dropDown, dropDown.width / 2, dropDown.height / 2);
            waitForRendering(rectCombo);
            compare(rectCombo.expanded, true, "combo is not expanded");
            tryCompareFunction(function() { return comboListPanel.opacity}, 1.0);

            mouseClick(rectCombo, rectCombo.width / 2, rectCombo.collapsedHeight / 2);
            waitForRendering(rectCombo);
            compare(rectCombo.expanded, false, "combo is not collapsed");
            tryCompareFunction(function() { return comboListPanel.opacity}, 0.0);
        }

        function test_flickRectCombo() {
            var dropDown = findChild(rectCombo, "combobutton_dropdown");
            var comboListPanel = findChild(rectCombo, "combobutton_combopanel");
            var comboList = findChild(rectCombo, "combobutton_combolist");
            mouseClick(dropDown, dropDown.width / 2, dropDown.height / 2);
            waitForRendering(rectCombo);
            tryCompareFunction(function() { return comboListPanel.opacity}, 1.0);
            verify(comboList.height > rectCombo.expandedHeight);
            // comboList flicker is the combolist parent's parent
            var comboListFlicker = comboList.parent.parent;
            compare(comboListFlicker.interactive, true, "combo list holder must be interactive");

            // drag the rectangle, the Flickable should be moving
            var x = comboListFlicker.width / 2;
            var y = comboListFlicker.height / 2;
            var dy = comboListFlicker.height / 6;
            spy.target = comboListFlicker;
            spy.signalName = "onMovementEnded";
            mouseDrag(comboListFlicker, x, y, x, dy);
            waitForRendering(comboListFlicker);
            compare(spy.count, 1, "combo list content did not move");
        }

        function test_autoExpandHeight() {
            var comboListPanel = findChild(columnCombo, "combobutton_combopanel");
            var comboList = findChild(columnCombo, "combobutton_combolist");
            columnCombo.expanded = true;
            waitForRendering(columnCombo);
            tryCompareFunction(function() { return comboListPanel.opacity}, 1.0);
            var comboListFlicker = comboList.parent.parent;
            compare(comboListFlicker.interactive, false, "combo list holder must not be interactive");
            compare(comboListFlicker.height, columnCombo.comboListHeight, "combo list height differs from the holder height");
        }

        function test_emptyComboExpanded() {
            var comboListPanel = findChild(combo, "combobutton_combopanel");
            var comboList = findChild(combo, "combobutton_combolist");
            combo.expanded = true;
            waitForRendering(combo);
            waitForRendering(comboListPanel);
            tryCompareFunction(function() { return comboListPanel.opacity}, 0.0, 1000);
        }

        function test_longCombo() {
            var comboListPanel = findChild(longCombo, "combobutton_combopanel");
            var comboList = findChild(longCombo, "combobutton_combolist");
            longCombo.expanded = true;
            waitForRendering(longCombo);
            waitForRendering(comboListPanel);
            tryCompareFunction(function() { return comboListPanel.opacity}, 1.0);
            verify(comboListPanel.height < longCombo.expandedHeight);
        }

        function test_listCombo() {
            var comboListPanel = findChild(listCombo, "combobutton_combopanel");
            var comboList = findChild(listCombo, "combobutton_combolist");
            listCombo.expanded = true;
            waitForRendering(listCombo);
            waitForRendering(comboListPanel);
            tryCompareFunction(function() { return comboListPanel.opacity}, 1.0);

            compare(list.height, comboList.height, "list and comboList height differs");
        }
    }
}

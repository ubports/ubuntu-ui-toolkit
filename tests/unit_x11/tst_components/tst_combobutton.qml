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
                anchors {
                    left: parent.left
                    right: parent.right
                }
            }
        }
    }

    UbuntuTestCase {
        name: "ComboButton"
        when: windowShown

        function cleanup() {
            rectCombo.expanded = false;
            columnCombo.expanded = false;
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

        function test_expandRectComboThroughProperty() {
            rectCombo.expanded = true;
            waitForRendering(rectCombo);
            var comboListHolder = findChild(rectCombo, "combolist_panel");
            tryCompareFunction(function() { return comboListHolder.opacity}, 1.0);
        }

        function test_expandRectComboThroughClick() {
            var dropDown = findChild(rectCombo, "dropdown_button");
            mouseClick(dropDown, dropDown.width / 2, dropDown.height / 2);
            waitForRendering(rectCombo);
            compare(rectCombo.expanded, true, "combo is not expanded");
            var comboListHolder = findChild(rectCombo, "combolist_panel");
            tryCompareFunction(function() { return comboListHolder.opacity}, 1.0);
        }

        function test_autoCollapse() {
            var dropDown = findChild(rectCombo, "dropdown_button");
            var comboListHolder = findChild(rectCombo, "combolist_panel");
            mouseClick(dropDown, dropDown.width / 2, dropDown.height / 2);
            waitForRendering(rectCombo);
            compare(rectCombo.expanded, true, "combo is not expanded");
            tryCompareFunction(function() { return comboListHolder.opacity}, 1.0);

            mouseClick(rectCombo, rectCombo.width / 2, rectCombo.collapsedHeight / 2);
            waitForRendering(rectCombo);
            compare(rectCombo.expanded, false, "combo is not collapsed");
            tryCompareFunction(function() { return comboListHolder.opacity}, 0.0);
        }

        function test_flickRectCombo() {
            var dropDown = findChild(rectCombo, "dropdown_button");
            var comboListHolder = findChild(rectCombo, "combolist_panel");
            var comboListContent = findChild(rectCombo, "combo_list");
            mouseClick(dropDown, dropDown.width / 2, dropDown.height / 2);
            waitForRendering(rectCombo);
            tryCompareFunction(function() { return comboListHolder.opacity}, 1.0);
            verify(comboListContent.height > rectCombo.expandedHeight);


            // drag the rectangle
        }
    }
}

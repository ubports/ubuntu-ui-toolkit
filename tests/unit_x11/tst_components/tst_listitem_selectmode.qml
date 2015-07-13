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
import Ubuntu.Components 1.2
import QtTest 1.0
import Ubuntu.Test 1.0

MainView {
    applicationName: "testapp.nik90"

    width: units.gu(100)
    height: units.gu(75)

    Page {
        title: i18n.tr("ListView Bug")

        UbuntuListView {
            id: listView

            ListModel {
                id: instructionModel
                ListElement { text: "Let's try to reproduce this listview bug ;)" }
                ListElement { text: "Step 1: Long press to active multiselect mode" }
                ListElement { text: "Step 2: Press on any checkbox to select that listitem" }
                ListElement { text: "Step 3: Now press on the list item of the checkbox you chose in the previous step." }
                ListElement { text: "Notice how it doesn't select/unselect it despite pressing it" }
                ListElement { text: "Also notice how when pressing on the listitem in step 3, the selected indice still changes!" }
            }

            anchors.fill: parent
            model: instructionModel

            delegate: ListItem {
                objectName: "listItem" + index
                height: units.gu(8)
                contentItem.anchors { leftMargin: units.gu(2); rightMargin: units.gu(2) }

                Label {
                    id: label
                    text: modelData
                    anchors.fill: parent
                    wrapMode: Text.WordWrap
                    verticalAlignment: Text.AlignVCenter
                }

                onPressAndHold: {
                    ListView.view.ViewItems.selectMode = !ListView.view.ViewItems.selectMode
                }

                onClicked: {
                    if (selectMode) {
                        selected = !selected
                    }
                }
            }

            footer: Label {
                anchors { left: parent.left; leftMargin: units.gu(2) }
                width: parent.width
                text: "Selected Indices: " + listView.ViewItems.selectedIndices
            }
        }
    }

    UbuntuTestCase {
        when: windowShown

        function test_select_toggle_breaks_property_binding_bug1469471() {
            listView.ViewItems.selectMode = true;
            wait(400);

            // select through checkbox
            var listItem = findChild(listView, "listItem0");
            verify(listItem);
            var panel = findChild(listItem, "selection_panel0");
            verify(panel);
            var checkBox = findChild(panel, "listitem_select");
            verify(checkBox);

            // check the box
            mouseClick(checkBox, 0, 0);
            compare(checkBox.checked, true);

            // now toggle selected
            listItem.selected = !listItem.selected;
            compare(checkBox.checked, false);
        }
    }
}


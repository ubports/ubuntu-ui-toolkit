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
import Ubuntu.Components 0.1

Item {
    width: units.gu(50)
    height: units.gu(80)

    MainView {
        id: mainViewHeader
        anchors.fill: parent

        Item {
            // Wraping the Page inside this Item should not
            // affect the header alignment, see bug #1261907.
            anchors.fill: parent

            Page {
                title: "listview"

                ListView {
                    anchors.fill: parent
                    id: listView

                    header: Rectangle {
                        color: "red"
                        width: parent.width
                        height: units.gu(5)
                    }
                    model: 500
                    delegate: Label {
                        text: "number " +index
                    }
                    onContentYChanged: print("contentY = "+contentY)
                }
            }
        }
    }

    TestCase {
        name: "HeaderAlignment"
        when: windowShown
        function test_ListViewHeaderAlignment_bug1202277_bug1261907() {
            compare(listView.contentY, -listView.headerItem.height - mainViewHeader.__propagated.header.height,
                    "ListView header is aligned with the MainView header");
        }
    }
}

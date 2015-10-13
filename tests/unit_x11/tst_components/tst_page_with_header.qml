/*
 * Copyright (C) 2015 Canonical Ltd.
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
import Ubuntu.Components 1.3
import Ubuntu.Test 1.0

MainView {
    width: units.gu(50)
    height: units.gu(70)
    id: root

    Item {
        id: invisible
        visible: false

        Rectangle {
            objectName: "my_rectangle"
            id: rect
            anchors {
                left: parent.left
                right: parent.right
            }
            height: units.gu(6)
            color: UbuntuColors.red
        }

        PageHeader {
            id: pageHeader
            objectName: "my_page_header"
            title: "Page header"
            trailingActionBar.actions: [
                Action {
                    iconName: "settings"
                    text: "First"
                },
                Action {
                    iconName: "info"
                    text: "Second"
                }
            ]
        }
    }

    Page {
        id: page
        title: "Page with header"
        header: pageHeader

        Column {
            anchors {
                left: parent.left
                right: parent.right
                top: parent.top
                topMargin: units.gu(10)
                leftMargin: units.gu(10)
                rightMargin: units.gu(10)
            }
            height: childrenRect.height

            Button {
                anchors {
                    left: parent.left
                    right: parent.right
                }
                text: "Page header"
                onClicked: page.header = pageHeader
            }
            Button {
                anchors {
                    left: parent.left
                    right: parent.right
                }
                text: "Rectangle"
                onClicked: page.header = rect
            }
            Button {
                anchors {
                    left: parent.left
                    right: parent.right
                }
                text: "Null"
                onClicked: page.header = null
            }
        }
    }

    UbuntuTestCase {
        name: "PageWithHeader"
        when: windowShown
        id: testCase

        function initTestCase() {

        }

        function test_header() {

        }

    }
}

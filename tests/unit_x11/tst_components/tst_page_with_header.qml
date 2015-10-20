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
            id: myRectangle
            anchors {
                left: parent.left
                right: parent.right
            }
            height: units.gu(6)
            color: UbuntuColors.red
        }

        PageHeader {
            id: myPageHeader
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
        header: myPageHeader

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
                onClicked: page.header = myPageHeader
            }
            Button {
                anchors {
                    left: parent.left
                    right: parent.right
                }
                text: "Rectangle"
                onClicked: page.header = myRectangle
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

        property var appHeader;
        function initTestCase() {
            var pageHeader = findChild(page, "my_page_header");
            compare(pageHeader, myPageHeader,
                    "My PageHeader not initialized with Page as its parent.");
            appHeader = findChild(root, "MainView_Header");
            compare(appHeader.visible, false,
                    "AppHeader is not hidden initially when Page.header is set.");
        }

        function cleanup() {
            page.header = myPageHeader;
            var pageHeader = findChild(page, "my_page_header");
            compare(pageHeader, myPageHeader,
                    "PageHeader is not correctly re-parented to the Page when setting Page.header.");
            compare(myPageHeader.visible, true,
                    "PageHeader is not visible after being re-parented to the Page.");
            compare(appHeader.visible, false,
                    "AppHeader is not hidden when Page.header is set.");
        }

        function test_page_with_no_header() {
            page.header = null;
            compare(myPageHeader.parent, invisible,
                    "Header parent is not correctly reverted when unsetting Page.header.");
            compare(myPageHeader.visible, false,
                    "My PageHeader is still visible after re-parenting it to an invisible Item.");
            compare(appHeader.visible, true,
                    "AppHeader does not become visible when Page.header is null.");
        }

        function test_page_with_alternative_header() {
            page.header = myRectangle;
            compare(myPageHeader.parent, invisible,
                    "Header parent not correctly reverted when setting a different Page.header.");
            compare(myRectangle.parent, page,
                    "Rectangle parent is not correctly set to page after setting it as Page.header.");
            compare(appHeader.visible, false,
                    "Setting a different Page.header Item shows the AppHeader.");

            page.header = myPageHeader;
            compare(myRectangle.parent, invisible,
                    "Rectangle parent is not correctly reverted after unsetting it as Page.header.");
            // myPageHeader parent is checked in cleanup().
        }
    }
}

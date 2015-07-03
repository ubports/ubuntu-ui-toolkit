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
import Ubuntu.Test 1.0
import Ubuntu.Components 1.3

Item {
    width: 400
    height: 600

    MainView {
        anchors.fill: parent
        id: mainView
        Page {
            id: page
            title: "Invisible title"
            head {
                visible: false
                locked: true
            }
            Label {
                text: "Top of the page"
                anchors {
                    top: parent.top
                }
            }
        }
    }

    UbuntuTestCase {
        id: testCase
        name: "PageNoHeader"
        when: windowShown

        function get_header_visible() {
            var header = findChild(mainView, "MainView_Header");
            return (header.y !== -header.height)
        }

        function test_header_hidden() {
            compare(get_header_visible(), false, "Header is not hidden.");
        }

        function test_page_height() {
            compare(page.height, mainView.height,
                    "Page height does not match MainView height.");
        }
    }
}

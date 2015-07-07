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

MainView {
    id: root
    width: units.gu(120)
    height: units.gu(71)

    MultiColumnView {
        id: multiColumnView
        width: parent.width
        height: parent.height
        primaryPage: rootPage

        Page {
            id: rootPage
            title: "Root"

            Column {
                anchors {
                    top: parent.top
                    left: parent.left
                    right: parent.right
                }
                height: childrenRect.height

                ListItemWithLabel {
                    text: "Add page left"
                    onClicked: multiColumnView.addPageToCurrentColumn(rootPage, leftPage)
                }
                ListItemWithLabel {
                    text: "Add page right"
                    onClicked: multiColumnView.addPageToNextColumn(rootPage, rightPage)
                }
                ListItemWithLabel {
                    text: "Add sections page right"
                    onClicked: multiColumnView.addPageToNextColumn(rootPage, sectionsPage)
                }
                ListItemWithLabel {
                    text: "Add external page right"
                    onClicked: multiColumnView.addPageToNextColumn(
                                   rootPage, Qt.resolvedUrl("MyExternalPage.qml"))
                }
            }
        }
        Page {
            id: leftPage
            title: "First column"
            Rectangle {
                anchors {
                    fill: parent
                    margins: units.gu(2)
                }
                color: "orange"
            }
        }
        Page {
            id: rightPage
            title: "Second column"
            Rectangle {
                anchors {
                    fill: parent
                    margins: units.gu(2)
                }
                color: "green"
            }
        }
        Page {
            id: sectionsPage
            title: "Page with sections"
            head.sections.model: ["uno", "dos", "tres"]

            Rectangle {
                anchors {
                    fill: parent
                    margins: units.gu(2)
                }
                color: "blue"
            }
        }
    }
    UbuntuTestCase {
        when: windowShown

        function get_number_of_columns() {
            var body = findChild(multiColumnView, "body");
            return body.children.length;
        }

        function get_header(index) {
            return findChild(multiColumnView, "Header" + index);
        }

        function get_number_of_headers() {
            // FIXME: With only one column, revert to using the AppHeader
            //  so multiColumnView sill not include any headers.
            var numHeaders = 0;
            var header = findChild(multiColumnView, "Header0");
            verify(header !== null, "No header found!");
            while (header !== null) {
                numHeaders++;
                header = get_header(numHeaders);
            }
            return numHeaders;
        }

        function cleanup() {
            multiColumnView.width = root.width;
            multiColumnView.height = root.height;
            multiColumnView.removePages(rootPage);
        }

        function test_number_of_headers_equals_number_of_columns() {
            multiColumnView.width = units.gu(40);
            compare(get_number_of_columns(), 1, "Number of columns is not 1.");
            compare(get_number_of_headers(), 1, "Number of headers is not 1.");
            multiColumnView.width = root.width;
            compare(get_number_of_columns(), 2, "Number of columns is not 2.");
            compare(get_number_of_headers(), 2, "Number of headers is not 2.");
        }

        function test_header_configuration_equals_column_page_configuration() {
            compare(get_number_of_headers(), 2, "Number of headers is not 2 initially.");
            compare(get_header(0).config, rootPage.head,
                    "First column header is not initialized with primaryPage header config.");
            compare(get_header(1).config, null,
                    "Second column header is not initalized with null.");

            multiColumnView.addPageToCurrentColumn(rootPage, leftPage);
            compare(get_header(0).config, leftPage.head,
                    "First column header is not updated properly.");
            compare(get_header(1).config, null,
                    "Second column header is updated when it should not be.");
            multiColumnView.removePages(leftPage);
            compare(get_header(0).config, rootPage.head,
                    "First column header is not reverted properly.");

            multiColumnView.addPageToNextColumn(rootPage, rightPage);
            compare(get_header(0).config, rootPage.head,
                    "First column header is updated when it should not be.");
            compare(get_header(1).config, rightPage.head,
                    "Second column header is not updated properly.");
            multiColumnView.removePages(rightPage);
            compare(get_header(1).config, null,
                    "Second column header is not reverted properly.");
        }

        function test_header_title_for_external_page() {
            multiColumnView.addPageToNextColumn(rootPage, Qt.resolvedUrl("MyExternalPage.qml"));
            compare(get_header(1).config.title, "Page from QML file",
                    "Adding external Page does not update the header title.");
        }

        function test_header_height() {
            // contentHeight + divider height
            var baseHeight = units.gu(6) + units.dp(1);
            var withSectionsHeight = baseHeight + units.gu(4);
            var n = get_number_of_headers();
            var i;
            for (i = 0; i < n; i++) {
                compare(get_header(i).height, baseHeight,
                        "Header " + i + " height is not initialized correctly.");
            }
            multiColumnView.addPageToNextColumn(rootPage, rightPage);
            for (i = 0; i < n; i++) {
                compare(get_header(i).height, baseHeight,
                        "Header " + i + " height is incorrect after adding Page.");
            }
            multiColumnView.removePages(rightPage);
            multiColumnView.addPageToNextColumn(rootPage, sectionsPage);
            for (i = 0; i < n; i++) {
                compare(get_header(i).height, withSectionsHeight,
                        "Header " + i + " height is incorrect after adding single Page with sections.");
            }
            multiColumnView.removePages(sectionsPage);
            for (i = 0; i < n; i++) {
                compare(get_header(i).height, baseHeight,
                        "Header " + i +
                        " height is not correctly reverted after removing Page with sections.");
            }
        }
    }
}

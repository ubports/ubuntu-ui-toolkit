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
                Button {
                    anchors.centerIn: parent
                    text: "right"
                    onTriggered: multiColumnView.addPageToNextColumn(leftPage, rightPage)
                }
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
                Button {
                    anchors.centerIn: parent
                    text: "Another page!"
                    onTriggered: multiColumnView.addPageToCurrentColumn(rightPage, sectionsPage)
                }
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
//        Page {
//            id: blackPage
//            title: "Black"
//            Rectangle {
//                anchors {
//                    fill: parent
//                    margins: units.gu(1)
//                }
//                color: "black"
//            }
//        }
    }
    UbuntuTestCase {
        when: windowShown

        function get_number_of_columns() {
            var body = findChild(multiColumnView, "body");
            return body.children.length;
        }

        function get_header(column) {
            return findChild(multiColumnView, "Header" + column);
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

        function get_back_button_visible(column) {
            var header = get_header(column);
            var back_button = findChild(header, "backButton");
            return back_button.visible;
        }

        function cleanup() {
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

        function test_back_button() {
            // A is first column, B is second column.
            // A:i, B:j = i pages in A, j pages in B.

            // primary page has no back button
            // A:1, B:0
            compare(get_back_button_visible(0), false,
                    "Back button is visible for primary page.");
            multiColumnView.addPageToCurrentColumn(rootPage, leftPage);
            // A:2, B:0
            compare(get_back_button_visible(0), true,
                    "Adding page 2 to column A does not show back button.");

            multiColumnView.removePages(leftPage);
            // A:1, B:0
            compare(get_back_button_visible(0), false,
                    "Removing page 2 from column A does not hide back button.");

            multiColumnView.addPageToNextColumn(rootPage, rightPage);
            // A:1, B:1
            compare(get_back_button_visible(0), false,
                    "Adding page 1 to column B shows back button in column A.");
            compare(get_back_button_visible(1), false,
                    "Adding page 1 to column B shows back button in column B.");

            multiColumnView.addPageToCurrentColumn(rootPage, leftPage);
            // A:2, B:1
            compare(get_back_button_visible(0), true,
                    "Adding page 2 to column A not show back button when column B has a page.");
            compare(get_back_button_visible(1), false,
                    "Adding page 2 to column A shows back button in column B.");
            multiColumnView.removePages(leftPage);
            // A:1, B:1

            multiColumnView.addPageToNextColumn(rootPage, sectionsPage);
            // A:1, B:2
            compare(get_back_button_visible(0), false,
                    "Adding page 2 to column B from column A shows back button in column A.");
            compare(get_back_button_visible(1), false,
                    "Adding page 2 to column B from column A shows back button in column B.");
            multiColumnView.removePages(sectionsPage);
            // A:1, B:1

            multiColumnView.addPageToCurrentColumn(rightPage, sectionsPage);
            // A:1, B:2
            compare(get_back_button_visible(0), false,
                    "Adding page 2 to column B from column B shows back button in column A.");
            compare(get_back_button_visible(1), true,
                    "Adding page 2 to column B from column B does not show back button in column B.");

            multiColumnView.addPageToCurrentColumn(rootPage, leftPage);
            // A:2, B:2
            compare(get_back_button_visible(0), true,
                    "Adding page 2 to column A does not show back button in column A when column B has 2 pages.");
            compare(get_back_button_visible(1), true,
                    "Adding page 2 to column A hides back button in column B.");

            multiColumnView.removePages(sectionsPage);
            // A:2, B:1
            compare(get_back_button_visible(0), true,
                    "Removing page 2 from column B hides back button in column A.");
            compare(get_back_button_visible(1), false,
                    "Removing page 2 from column B does not hide back button when column A has 2 pages.");
        }

//            multiColumnView.addPageToCurrentColumn(rightPage, sectionsPage);
//            compare(get_back_button_visible(0), false,
//                    "Adding second page to the second column shows back button in first column.");
//        }

//        function test_back_button_next_column() {
//            multiColumnView.addPageToNextColumn(rootPage, rightPage);
//            compare(get_back_button_visible(1), false,
//                    "First page on second column shows a back button.");
//            multiColumnView.addPageToCurrentColumn(rightPage, sectionsPage);
//            compare(get_back_button_visible(1), true,
//                    "Adding a second page on the second column does not show back button.");
//        }
//        function test_no_back_button() {
//            // primary page has no back button
//            compare(get_back_button_visible(0), false,
//                    "Back button is visible for primary page.");
//            multiColumnView.addPageToNextColumn(rootPage, rightPage);
//            compare(get_back_button_visible(1), false,
//                    "Back button is visible for first page added to column.");
//            multiColumnView.addPageToCurrentColumn(rootPage, leftPage);
//            compare(get_back_button_visible(1), false,
//                    "Adding a second page to the first column shows back button in the second column.");
//        }

//        function test_show_back_button() {
//            multiColumnView.addPageToCurrentColumn(rootPage, leftPage);
//            compare(get_back_button_visible(0), true,
//                    "Back button not visible for second page in first column.");
//            multiColumnView.removePages(leftPage);
//            multiColumnView.addPageToNextColumn(rootPage, rightPage);
//            multiColumnView.addPageToCurrentColumn(rightPage, sectionsPage);
//            compare(get_back_button_visible(1), true,
//                    "Back button is not visible for second page in second column.");
//        }

    }
}

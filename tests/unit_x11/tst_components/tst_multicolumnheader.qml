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

    // 2 on desktop, 1 on phone.
    property int columns: width >= units.gu(80) ? 2 : 1

    AdaptivePageLayout {
        id: layout
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
                    onClicked: layout.addPageToCurrentColumn(rootPage, leftPage)
                }
                ListItemWithLabel {
                    text: "Add page right"
                    onClicked: layout.addPageToNextColumn(rootPage, rightPage)
                }
                ListItemWithLabel {
                    text: "Add sections page right"
                    onClicked: layout.addPageToNextColumn(rootPage, sectionsPage)
                }
                ListItemWithLabel {
                    text: "Add external page right"
                    onClicked: layout.addPageToNextColumn(
                                   rootPage, Qt.resolvedUrl("MyExternalPage.qml"))
                }
                ListItemWithLabel {
                    text: "Add page with head contents left"
                    onClicked: layout.addPageToCurrentColumn(rootPage, headContentsPage)
                }
                ListItemWithLabel {
                    text: "Add page with head contents right"
                    onClicked: layout.addPageToNextColumn(rootPage, headContentsPage)
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
                    onTriggered: layout.addPageToNextColumn(leftPage, rightPage)
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
                    onTriggered: layout.addPageToCurrentColumn(rightPage, sectionsPage)
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
        Page {
            id: headContentsPage
            head.contents: Rectangle {
                id: headRectangle
                color: UbuntuColors.orange
                anchors.fill: parent
            }
        }
    }

    UbuntuTestCase {
        when: windowShown

        function resize_single_column_width() {
            layout.width = units.gu(40);
        }

        // resize to use the full window width
        function resize_full_width() {
            layout.width = root.width;
        }

        function get_number_of_columns() {
            var body = findChild(layout, "body");
            return body.children.length;
        }

        function get_header(column) {
            return findChild(layout, "Header" + column);
        }

        function get_number_of_headers() {
            // FIXME: With only one column, revert to using the AppHeader
            //  so layout will not include any headers.
            var numHeaders = 0;
            var header = get_header(0);
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
            layout.removePages(rootPage);
            resize_full_width();
        }

        function test_number_of_headers_equals_number_of_columns_wide() {
            if (root.columns !== 2) {
                skip("Only for wide view.");
            }
            compare(get_number_of_columns(), 2, "Number of columns is not 2.");
            compare(get_number_of_headers(), 2, "Number of headers is not 2.");
        }

        function test_number_of_headers_equals_number_of_columns_narrow() {
            if (root.columns !== 1) {
                resize_single_column_width();
            }
            compare(get_number_of_columns(), 1, "Number of columns is not 1 on narrow screen.");
            compare(get_number_of_headers(), 1, "Number of headers is not 1 on narrow screen.");
        }

        function test_header_configuration_equals_column_page_configuration_wide() {
            if (root.columns !== 2) {
                skip("Only for wide view.");
            }
            compare(get_number_of_headers(), 2, "Number of headers is not 2 initially.");
            compare(get_header(0).config, rootPage.head,
                    "First column header is not initialized with primaryPage header config.");
            compare(get_header(1).config, null,
                    "Second column header is not initalized with null.");

            layout.addPageToCurrentColumn(rootPage, leftPage);
            compare(get_header(0).config, leftPage.head,
                    "First column header is not updated properly.");
            compare(get_header(1).config, null,
                    "Second column header is updated when it should not be.");
            layout.removePages(leftPage);
            compare(get_header(0).config, rootPage.head,
                    "First column header is not reverted properly.");

            layout.addPageToNextColumn(rootPage, rightPage);
            compare(get_header(0).config, rootPage.head,
                    "First column header is updated when it should not be.");
            compare(get_header(1).config, rightPage.head,
                    "Second column header is not updated properly.");
            layout.removePages(rightPage);
            compare(get_header(1).config, null,
                    "Second column header is not reverted properly.");
        }

        function test_header_configuration_equals_column_page_configuration_narrow() {
            if (root.columns !== 1) {
                resize_single_column_width();
            }
            compare(get_number_of_headers(), 1, "Number of headers is not 1.");
            compare(get_header(0).config, rootPage.head,
                    "First column header is not initialized with primaryPage header config.");

            layout.addPageToCurrentColumn(rootPage, leftPage);
            compare(get_header(0).config, leftPage.head,
                    "Single column header is not updated properly.");
            layout.removePages(leftPage);
            compare(get_header(0).config, rootPage.head,
                    "Single column header is not reverted properly.");

            layout.addPageToNextColumn(rootPage, rightPage);
            compare(get_header(0).config, rightPage.head,
                    "Single column header is not updated properly when adding to next column.");
            layout.removePages(rightPage);
            compare(get_header(0).config, rootPage.head,
                    "Single column header is not reverted properly after adding to next column.");
        }

        function test_header_title_for_external_page() {
            var incubator = layout.addPageToNextColumn(rootPage, Qt.resolvedUrl("MyExternalPage.qml"));
            var pageLoaded = false;
            incubator.onStatusChanged = function (status) {
                pageLoaded = (incubator.object != null);
            }
            tryCompareFunction(function() {return pageLoaded}, true, 1000);
            var n = root.columns === 2 ? 1 : 0
            compare(get_header(n).config.title, "Page from QML file",
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
            layout.addPageToNextColumn(rootPage, rightPage);
            for (i = 0; i < n; i++) {
                compare(get_header(i).height, baseHeight,
                        "Header " + i + " height is incorrect after adding Page.");
            }
            layout.removePages(rightPage);
            layout.addPageToNextColumn(rootPage, sectionsPage);
            for (i = 0; i < n; i++) {
                compare(get_header(i).height, withSectionsHeight,
                        "Header " + i + " height is incorrect after adding single Page with sections.");
            }
            layout.removePages(sectionsPage);
            for (i = 0; i < n; i++) {
                compare(get_header(i).height, baseHeight,
                        "Header " + i +
                        " height is not correctly reverted after removing Page with sections.");
            }
        }

        function test_back_button_wide() {
            if (root.columns !== 2) {
                skip("Only for wide view.");
            }
            // A is the first column, B is the second column.
            // A:i, B:j = i pages in A, j pages in B.

            // primary page has no back button
            // A:1, B:0
            compare(get_back_button_visible(0), false,
                    "Back button is visible for primary page.");
            layout.addPageToCurrentColumn(rootPage, leftPage);
            // A:2, B:0
            compare(get_back_button_visible(0), true,
                    "Adding page 2 to column A does not show back button.");

            layout.removePages(leftPage);
            // A:1, B:0
            compare(get_back_button_visible(0), false,
                    "Removing page 2 from column A does not hide back button.");

            layout.addPageToNextColumn(rootPage, rightPage);
            // A:1, B:1
            compare(get_back_button_visible(0), false,
                    "Adding page 1 to column B shows back button in column A.");
            compare(get_back_button_visible(1), false,
                    "Adding page 1 to column B shows back button in column B.");

            layout.addPageToCurrentColumn(rootPage, leftPage);
            // A:2, B:0
            compare(get_back_button_visible(0), true,
                    "Adding page 2 to column A not show back button when column B has a page.");
            layout.removePages(leftPage);
            // A:1, B:0

            layout.addPageToNextColumn(rootPage, rightPage);
            layout.addPageToCurrentColumn(rightPage, sectionsPage);
            // A:1, B:2
            compare(get_back_button_visible(0), false,
                    "Adding page 2 to column B shows back button in column A.");
            compare(get_back_button_visible(1), true,
                    "Adding page 2 to column B does not show back button in column B.");

            layout.addPageToCurrentColumn(rootPage, leftPage);
            // A:2, B:0
            layout.addPageToNextColumn(leftPage, rightPage);
            layout.addPageToCurrentColumn(rightPage, sectionsPage);
            compare(get_back_button_visible(0), true,
                    "Adding page 2 to column A does not show back button in column A when column B has 2 pages.");
            compare(get_back_button_visible(1), true,
                    "Adding page 2 to column A hides back button in column B.");

            layout.removePages(sectionsPage);
            // A:2, B:1
            compare(get_back_button_visible(0), true,
                    "Removing page 2 from column B hides back button in column A.");
            compare(get_back_button_visible(1), false,
                    "Removing page 2 from column B does not hide back button when column A has 2 pages.");
        }

        function test_back_button_narrow() {
            if (root.columns !== 1) {
                resize_single_column_width();
            }

            compare(get_back_button_visible(0), false,
                    "Back button is visible for primary page.");
            layout.addPageToCurrentColumn(rootPage, leftPage);
            compare(get_back_button_visible(0), true,
                    "No back button visible with two pages in single column.");
            layout.removePages(leftPage);
            compare(get_back_button_visible(0), false,
                    "Back button remains visible after removing second page from column.");

            layout.addPageToNextColumn(rootPage, rightPage);
            compare(get_back_button_visible(0), true,
                    "No back button visible after pushing to next column when viewing single column.");
            layout.removePages(rightPage);
            compare(get_back_button_visible(0), false,
                    "Back button remains visible after removing page from following column.");
        }

        function test_head_contents_visible_bug1488922() {
            layout.addPageToCurrentColumn(rootPage, headContentsPage);
            // The bug occurred when the style for the subheader was unset
            //  after a short delay, so wait before testing Page.head.contents.parent.
            wait(100);
            compare(headRectangle.visible, true, "Head contents not visible in current column.");
            compare(headRectangle.parent == null, false, "Head contents has no parent in current column.");
            layout.removePages(headContentsPage);
            layout.addPageToNextColumn(rootPage, headContentsPage);
            wait(100);
            compare(headRectangle.visible, true, "Head contents not visible in next column.");
            compare(headRectangle.parent == null, false, "Head contents has no parent in next column.");
            layout.removePages(headContentsPage);
        }
    }
}

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
import QtTest 1.0
import Ubuntu.Test 1.0
import Ubuntu.Components 1.3

MainView {
    id: root
    width: units.gu(120)
    height: units.gu(71)

    // 2 on desktop, 1 on phone.
    property int columns: width >= units.gu(80) ? 2 : 1

    Column {
        anchors.fill: parent
        AdaptivePageLayout {
            id: layout
            width: parent.width
            height: parent.height / 2

            primaryPage: page1

            Page {
                id: page1
                objectName: title
                title: "Page1"

                Column {
                    anchors.centerIn: parent
                    width: childrenRect.width
                    Button {
                        text: "Page 2 left"
                        onTriggered: layout.addPageToCurrentColumn(page1, page2)
                    }
                    Button {
                        text: "Page 3 right"
                        onTriggered: layout.addPageToNextColumn(page1, page3);
                    }
                }
            }
            Page {
                id: page2
                objectName: title
                title: "Page2"
            }
            Page {
                id: page3
                objectName: title
                title: "Page3"
            }
            Page {
                id: page4
                objectName: title
                title: "Page4"
            }
        }
        AdaptivePageLayout {
            id: defaults
            width: parent.width
            height: parent.height / 2
            Page {
                id: otherPage1
                objectName: title
                title: "Page1"
            }
            Page {
                id: otherPage2
                objectName: title
                title: "Page2"
            }
            Page {
                id: otherPage3
                objectName: title
                title: "Page3"
            }
        }
    }

    Component {
        id: pageComponent
        Page {
            objectName: title
            title: "DynamicPage"
        }
    }

    UbuntuTestCase {
        id: testCase
        when: windowShown

        signal pageLoaded()

        SignalSpy {
            id: loadedSpy
            target: testCase
            signalName: "pageLoaded"
        }
        SignalSpy {
            id: primaryPageSpy
            signalName: "primaryPageChanged"
        }

        function resize_single_column() {
            layout.width = units.gu(40);
        }

        // resize to use the full window width
        function resize_multiple_columns() {
            layout.width = root.width;
        }

        function getColumnHolder(apl, column) {
            verify(apl.columns > column);
            return findChild(apl, "ColumnHolder" + column);
        }

        function getPageWrapper(apl, page) {
            verify(apl.__d);
            return apl.__d.getWrapper(page);
        }

        function findPageFromLayout(apl, objectName) {
            var body = findChild(apl, "body");
            verify(body);
            return findChild(body, objectName);
        }

        function cleanup() {
            page1.title = "Page1";
            page2.title = "Page2";
            page3.title = "Page3";
            page4.title = "Page4";
            loadedSpy.clear();
            primaryPageSpy.clear();
            primaryPageSpy.target = null;
            resize_multiple_columns();
            layout.removePages(layout.primaryPage);
            defaults.primaryPage = null;
            wait(200);
        }

        function initTestCase() {
            compare(defaults.primaryPage, null, "primaryPage not null by default");
            compare(defaults.primaryPageSource, undefined, "primaryPageSource not set by default");
            compare(defaults.layouts.length, 0, "no layouts by default");
            compare(defaults.columns, columns, columns + " column(s) as default");

            verify(page1.active, "Page1 is not active but should be! Bug(1535241)");
            verify(!page2.active, "Page2 is active but should not be! Bug(1535241)");
        }

        function test_change_primaryPage() {
            defaults.primaryPage = otherPage1;
            defaults.addPageToCurrentColumn(defaults.primaryPage, otherPage2);
            defaults.primaryPage = otherPage3;
            verify(!findPageFromLayout(defaults, "Page2"), "Page2 still in the view!");
        }

        function test_add_page_when_source_page_not_in_stack() {
            ignoreWarning("sourcePage must be added to the view to add new page.");
            layout.addPageToCurrentColumn(page2, page3);
            ignoreWarning("sourcePage must be added to the view to add new page.");
            layout.addPageToNextColumn(page2, page3);
        }

        function test_add_page_with_null_sourcePage() {
            ignoreWarning("No sourcePage specified. Page will not be added.")
            layout.addPageToCurrentColumn(null, page1);
            ignoreWarning("No sourcePage specified. Page will not be added.")
            layout.addPageToNextColumn(null, page2);
        }

        function test_add_same_page_twice() {
            layout.addPageToCurrentColumn(page1, page2);
            layout.addPageToCurrentColumn(page2, page3);
            ignoreWarning("Cannot add a Page that was already added.");
            layout.addPageToCurrentColumn(page3, page2);
            ignoreWarning("Cannot add a Page that was already added.");
            layout.addPageToNextColumn(page3, page2);
        }

        function test_page_visible() {
            // Two columns on desktop, one on phone
            compare(page1.visible, true, "Primary page not initially visible.");
            compare(page2.visible, false, "Page 2 visible before it was added.");
            compare(page3.visible, false, "Page 3 visible before it was added.");

            layout.addPageToCurrentColumn(page1, page2);
            compare(page1.visible, false, "Page still visible after adding new page in current column.");
            compare(page2.visible, true, "Page invisible after adding it to current column.");
            layout.addPageToNextColumn(page2, page3);
            if (root.columns === 2) {
                compare(page2.visible, true, "Page in first column became invisible after adding to next column.");
            } else { // root.columns === 1
                compare(page2.visible, false, "Page in single column still visible after adding page to next column.");
            }
            compare(page3.visible, true, "Page invisible after adding it to next column.");

            // One column
            resize_single_column();
            compare(page3.visible, true, "Top page in last column invisible when resizing to one column.");
            compare(page2.visible, false, "Top page in first column visible when resizing to one column.");

            layout.removePages(page3);
            compare(page3.visible, false, "Page 3 visible after it was removed.");
            compare(page2.visible, true, "New top page in single column not visible.");

            layout.removePages(page1);
            compare(page1.visible, true, "Primary page not visible in single column.");
            compare(page2.visible, false, "Page 2 visible while it is not added.");
            layout.addPageToNextColumn(page1, page4);
            compare(page1.visible, false, "Page remains visible after adding to next column in single column view.");
            compare(page4.visible, true, "Page added to next column with single column view is not visible.");

            // Two columns on desktop, one on phone
            resize_multiple_columns();
            if (root.columns === 2) {
                compare(page1.visible, true, "Page in left column did not become visible when switching to multi-column view.");
                compare(page4.visible, true, "Page in right column became invisible when switching to multi-column view.");
            }
        }

        function test_add_to_current_prunes_next() {
            layout.addPageToNextColumn(page1, page2);
            compare(page2.visible, true, "Adding page to next column does not show that page.");
            layout.addPageToCurrentColumn(page1, page3);
            compare(page2.visible, false, "Adding page to current column does not clear next column.");
            layout.removePages(page3);
            compare(page2.visible, false, "Removing page from first column shows previous page in next column.");
        }

        function test_add_to_next_first_prunes_next() {
            layout.addPageToNextColumn(page1, page2);
            layout.addPageToNextColumn(page1, page3);
            layout.removePages(page3);
            compare(page2.visible, false, "Adding page to next column did not prune that column.");
        }

        function test_add_page_to_current_does_not_prune_current() {
            layout.addPageToCurrentColumn(page1, page2);
            layout.addPageToCurrentColumn(page2, page3);
            layout.removePages(page3);
            compare(page2.visible, true, "Adding page to current column pruned that column.");
        }

        function test_add_page_to_current_data() {
            return [
                {tag: "Synchronously to current column", func: "addPageToCurrentColumn", page: page2, params: {}, sync: true},
                {tag: "Synchronously to current column, with params", func: "addPageToCurrentColumn", page: page2, params: {title: "Altered title"}, sync: true},
                {tag: "Synchronously to next column", func: "addPageToNextColumn", page: page2, params: {}, sync: true},
                {tag: "Synchronously to next column, with params", func: "addPageToNextColumn", page: page2, params: {title: "Altered title"}, sync: true},
                {tag: "Asynchronously to current column", func: "addPageToCurrentColumn", page: pageComponent, params: {}, sync: false},
                {tag: "Asynchronously to current column, with params", func: "addPageToCurrentColumn", page: pageComponent, params: {title: "Altered title"}, sync: false},
                {tag: "Asynchronously to next column", func: "addPageToNextColumn", page: pageComponent, params: {}, sync: false},
                {tag: "Asynchronously to next column, with params", func: "addPageToNextColumn", page: pageComponent, params: {title: "Altered title"}, sync: false},
            ]
        }
        function test_add_page_to_current(data) {
            var incubator = layout[data.func](layout.primaryPage, data.page, data.params);
            compare((incubator == null), data.sync);
            if (incubator) {
                compare(incubator.status, Component.Loading, "The incubator status is not Component.Loading, but " + incubator.status);
                // for cleanup, we must wait till the page is added, so we can do proper cleanup
                incubator.onStatusChanged = function (status) {
                    // test params
                    for (var param in data.params) {
                        compare(incubator.object[param], data.params[param], "parameter '" + param + "' values do not match");
                    }
                    testCase.pageLoaded();
                    layout.removePages(layout.primaryPage);
                }
                // make sure we wait enough to let additional tests to complete
                loadedSpy.wait(1500);
            }
        }

        function test_asynchronous_page_loading_incubator_forcecompletion() {
            var incubator = layout.addPageToCurrentColumn(layout.primaryPage, pageComponent);
            verify(incubator, "Page added synchronously!");
            compare(incubator.status, Component.Loading, "Incubator in different state");
            incubator.forceCompletion();
            compare(incubator.status, Component.Ready, "Incubator not completed");
            verify(incubator.object, "Page object not set");
        }

        SignalSpy {
            id: widthSpy
            signalName: "widthChanged"
        }
        SignalSpy {
            id: heightSpy
            signalName: "heightChanged"
        }
        function test_hidden_page_keeps_geometry_bug1492343() {
            widthSpy.target = heightSpy.target = layout.primaryPage;
            layout.addPageToCurrentColumn(layout.primaryPage, page2);
            expectFailContinue("", "no width change expected");
            widthSpy.wait(400);
            expectFailContinue("", "no height change expected");
            heightSpy.wait(400);
        }

        SignalSpy {
            id: pageLoadedSpy
            signalName: "pageLoaded"
        }
        function test_old_page_disappears_when_new_one_ready_bug1499178_data() {
            return [
                {tag: "CurrentColumn, wide",
                            width: units.gu(120), nextColumn: false, page: pageComponent, sourcePage: layout.primaryPage},
                {tag: "CurrentColumn, narrow",
                            width: units.gu(40), nextColumn: false, page: pageComponent, sourcePage: layout.primaryPage},
                {tag: "NextColumn, wide", intermediatePage: page2,
                            width: units.gu(120), nextColumn: true, page: pageComponent, sourcePage: layout.primaryPage},
                {tag: "NextColumn, narrow", intermediatePage: page2,
                            width: units.gu(40), nextColumn: true, page: pageComponent, sourcePage: layout.primaryPage},
            ];
        }
        function test_old_page_disappears_when_new_one_ready_bug1499178(data) {
            layout.width = data.width;
            var wrapper = getPageWrapper(layout, data.sourcePage);
            verify(wrapper);
            var testColumn = MathUtils.clamp(wrapper.column + (data.nextColumn ? 1 : 0), 0, layout.columns - 1);
            var testHolder = getColumnHolder(layout, testColumn);
            verify(testHolder);

            if (data.intermediatePage) {
                layout.addPageToNextColumn(data.sourcePage, data.intermediatePage);
            }

            var testPage = testHolder.pageWrapper.object;
            var prevPageActive = false;
            var incubator = data.nextColumn
                        ? layout.addPageToNextColumn(data.sourcePage, data.page)
                        : layout.addPageToCurrentColumn(data.sourcePage, data.page);
            verify(incubator);
            compare(testHolder.pageWrapper.object, testPage);
            incubator.onStatusChanged = function (status) {
                if (status == Component.Ready) {
                    prevPageActive = testHolder.pageWrapper.object == testPage;
                    testCase.pageLoaded();
                }
            }
            loadedSpy.wait(2500);
            verify(prevPageActive);
        }

        function test_primaryPageSource_bug1499179_data() {
            return [
                {tag: "Component", test: pageComponent},
                {tag: "Document", test: Qt.resolvedUrl("MyExternalPage.qml")},
            ];
        }
        function test_primaryPageSource_bug1499179(data) {
            primaryPageSpy.target = defaults;
            defaults.primaryPageSource = data.test;
            primaryPageSpy.wait();
        }

        function test_change_primaryPageSource_data() {
            return [
                {tag: "Component", test: pageComponent, nextValue: Qt.resolvedUrl("MyExternalPage.qml")},
                {tag: "Document", test: Qt.resolvedUrl("MyExternalPage.qml"), nextValue: pageComponent},
            ];
        }
        function test_change_primaryPageSource(data) {
            primaryPageSpy.target = defaults;
            verify(defaults.primaryPage == null);
            verify(defaults.primaryPageSource == undefined);
            defaults.primaryPageSource = data.test;
            primaryPageSpy.wait(400);
            // add some pages
            defaults.addPageToCurrentColumn(defaults.primaryPage, otherPage2);
            // then replace the primaryPageSource
            primaryPageSpy.clear();
            defaults.primaryPageSource = data.nextValue;
            primaryPageSpy.wait(400);
            // look after page2
            verify(!findPageFromLayout(defaults, "Page2"), "Page2 still in the view!");
        }

        function test_primaryPageSource_precedence_over_primaryPage() {
            primaryPageSpy.target = defaults;
            defaults.primaryPage = otherPage1;
            primaryPageSpy.wait(400);
            // now set a value to primaryPageSource
            primaryPageSpy.clear();
            defaults.primaryPageSource = pageComponent;
            primaryPageSpy.wait(400);
        }

        function test_primaryPage_change_clears_primaryPageSource() {
            primaryPageSpy.target = defaults;
            defaults.primaryPageSource = pageComponent;
            primaryPageSpy.wait(400);
            compare(defaults.primaryPage.title, "DynamicPage", "DynamicPage not set as primaryPage");
            // now set a value to primaryPage
            primaryPageSpy.clear();
            defaults.primaryPage = otherPage1;
            primaryPageSpy.wait(400);
            compare(defaults.primaryPageSource, undefined, "primaryPageSource must be cleared");
        }
    }
}

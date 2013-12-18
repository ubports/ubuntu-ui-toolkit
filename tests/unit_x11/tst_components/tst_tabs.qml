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
    id: testCase
    width: units.gu(50)
    height: units.gu(80)

    Tabs {
        id: emptyTabs
    }

    Component {
        id: dynamicTab
        Tab{
            title: "OriginalTitle"
        }
    }

    MainView {
        id: mainView
        anchors.fill: parent
        Tabs {
            id: tabs
            Tab {
                id: tab1
                title: "tab 1"
                page: Page {
                    id: page1
                    Button {
                        id: button
                        anchors.centerIn: parent
                        text: "click"
                    }
                }
            }
            Tab {
                id: tab2
                title: "tab 2"
                page: Page {
                    id: page2
                }
            }
            Tab {
                id: tab3
                title: "tab 3"
                page: Page {
                    id: page3
                }
            }
            Tab {
                id: tabFlick1
                title: "flick"
                page: Page {
                    Flickable {
                        id: flickable1
                        anchors.fill: parent
                    }
                }
            }
            Tab {
                id: tabFlick2
                title: "flick 2"
                page: Page {
                    Flickable {
                        id: flickable2
                        anchors.fill: parent
                    }
                }
            }
            Tab {
                id: tabFlickLoader
                title: "load"
                page: Loader {
                    id: loader
                    sourceComponent: tabs.selectedTabIndex != 5 ? null : pageComponent
                }
            }
        }
        Component {
            id: pageComponent
            Page {
                title: "Loaded page"
                property Flickable flick: loadedFlickable
                Flickable {
                    id: loadedFlickable
                    anchors.fill: parent
                    contentHeight: 1000
                }
            }
        }
    }

    ListModel {
        id: inputModel
        Component.onCompleted: {
            append({ "name": "tab 1" });
            insert(0, { "name": "tab 0" });
            append({ "name": "tab 3" });
            insert(2, { "name": "tab 2" });
        }
    }

    Tabs {
        id: tabsWithRepeater
        Repeater {
            objectName: "tabsRepeater"
            id: tabsRepeater
            model: inputModel
            Tab {
                title: name
            }
        }
    }

    Tabs {
        id: repeaterTabs

        Repeater {
            objectName: "repeater"
            id: repeater
            Tab {
                title: modelData
            }
        }
    }

    Tabs {
        id: twoRepeaters

        Repeater {
            objectName: "firstRepeater"
            id: firstRepeater
            model: inputModel
            Tab {
                title: name
            }
        }

        Repeater {
            objectName: "secondRepeater"
            id: secondRepeater
            model: testCase.listModel
            Tab {
                title: modelData
            }
        }
    }

    property var listModel: ["tab #0", "tab #1", "tab #2", "tab #3"];

    Tabs {
        id: twinRepeaters
        ListModel {
            id: twinModel
            Component.onCompleted: {
                append({ "name": "twintab 1" });
                insert(0, { "name": "twintab 0" });
                append({ "name": "twintab 3" });
                insert(2, { "name": "twintab 2" });
            }
        }
        Repeater {
            objectName: "tabsRepeater"
            id: twinRepeater1
            model: twinModel
            Tab {
                title: name
            }
        }
        Repeater {
            objectName: "tabsRepeater"
            id: twinRepeater2
            model: twinModel
            Tab {
                title: name
            }
        }
    }

    TestCase {
        name: "TabsAPI"
        when: windowShown

        /*
          The following testcases are all related to bug #1253804
          */
        function test_tabOrder_bug1253804() {
            var tabsModel = tabsWithRepeater.tabBar.model;

            compare(tabsRepeater.count, inputModel.count, "Incorrect number of tabs in Tabs");
            compare(tabsModel.count, tabsRepeater.count, "Incorrect number of tabs in TabBar");
            for (var i=0; i < tabsModel.count; i++) {
                compare(tabsModel.get(i).title, inputModel.get(i).name, "Tab titles don't match for index "+i);
            }

            //shufle
            inputModel.move(1, 2, 1);
            inputModel.move(3, 0, 1);
            inputModel.move(1, 3, 1);
            // wait few miliseconds
            wait(50);
            for (i=0; i < tabsModel.count; i++) {
                compare(tabsModel.get(i).title, inputModel.get(i).name, "Tab titles after shuffling don't match for index "+i);
            }

            // set it to null
            tabsRepeater.model = null;
            compare(tabsWithRepeater.tabBar.model.count, 0, "There are still tabs left after repeater model is reset");
        }

        function test_repeaterTabs() {
            repeater.model = inputModel;
            var tabsModel = repeaterTabs.tabBar.model;

            compare(repeater.count, inputModel.count, "Incorrect number of tabs in Tabs");
            compare(tabsModel.count, repeater.count, "Incorrect number of tabs in TabBar");
            for (var i=0; i < tabsModel.count; i++) {
                compare(tabsModel.get(i).title, inputModel.get(i).name, "Tab titles don't match for index "+i);
            }

            // clear repeaterTabs
            repeater.model = null;
            compare(repeaterTabs.tabBar.model.count, 0, "There are still tabs left after repeater model is reset");
        }

        function test_repeaterTabs_arrayAsModel() {
            repeater.model = testCase.listModel;
            var tabsModel = repeaterTabs.tabBar.model;

            compare(repeater.count, testCase.listModel.length, "Incorrect number of tabs in Tabs");
            compare(tabsModel.count, repeater.count, "Incorrect number of tabs in TabBar");
            for (var i=0; i < tabsModel.count; i++) {
                compare(tabsModel.get(i).title, testCase.listModel[i], "Tab titles don't match for index "+i);
            }

            // shuffling elements in an array is not detectable in Repeater
            var x = testCase.listModel[1];
            testCase.listModel[1] = testCase.listModel[0];
            testCase.listModel[0] = x;
            expectFailContinue("", "Array changes are not detected by repeaters");
            compare(tabsModel.get(0).title, testCase.listModel[0], "Tab titles don't match for index 0");
            expectFailContinue("", "Array changes are not detected by repeaters");
            compare(tabsModel.get(1).title, testCase.listModel[1], "Tab titles don't match for index 0");

            // clear repeaterTabs
            repeater.model = null;
            compare(repeaterTabs.tabBar.model.count, 0, "There are still tabs left after repeater model is reset");

        }

        function test_twoRepeaters() {
            var tabsModel = twoRepeaters.tabBar.model;
            var secondRepeaterModel = secondRepeater.model;

            compare(tabsModel.count, firstRepeater.count + secondRepeater.count, "Incorrect number of tabs in TabBar");
            for (var i = 0; i < firstRepeater.count; i++) {
                compare(tabsModel.get(i).title, inputModel.get(i).name, "Tab titles don't match for index "+i);
            }
            for (i = firstRepeater.count; i < firstRepeater.count + secondRepeater.count; i++) {
                compare(tabsModel.get(i).title, secondRepeaterModel[i - firstRepeater.count], "Tab titles don't match for index "+i);
            }
        }

        function test_twinRepeaters() {
            var tabsModel = twinRepeaters.tabBar.model;

            compare(twinRepeater1.count, twinModel.count, "Incorrect number of tabs in the first repeater");
            compare(twinRepeater2.count, twinModel.count, "Incorrect number of tabs in the second repeater");
            compare(tabsModel.count, twinRepeater1.count + twinRepeater2.count, "Incorrect number of tabs in TabBar");
            for (var j = 0; j < 2; j++) {
                for (var i = 0; i < twinModel.count; i++) {
                    var index = j * twinModel.count + i;
                    compare(tabsModel.get(index).title, twinModel.get(i).name, "Tab titles don't match for Tabs index " + index);
                }
            }

            //shuffle
            twinModel.move(1, 2, 1);
            twinModel.move(3, 0, 1);
            twinModel.move(1, 3, 1);
            // wait few miliseconds till Tabs update is realized
            wait(50);
            for (var j = 0; j < 2; j++) {
                for (var i = 0; i < twinModel.count; i++) {
                    var index = j * twinModel.count + i;
                    compare(tabsModel.get(index).title, twinModel.get(i).name, "Tab titles don't match for Tabs index " + index);
                }
            }

            // set it to null
            twinRepeater1.model = null;
            twinRepeater2.model = null;
            compare(twinRepeaters.tabBar.model.count, 0, "There are still tabs left after repeater model is reset");
        }

        function test_emptyTabs() {
            compare(emptyTabs.selectedTabIndex, -1, "The default value for selectedTabIndex is -1 when there are no tabs");
            compare(emptyTabs.selectedTab, null, "The default selected tab is null when there are no tabs");
            compare(emptyTabs.currentPage, null, "The default currentPage is null when there are no tabs");
        }

        function test_tabsDefaults() {
            compare(tabs.selectedTabIndex, 0, "The default selectedTabIndex is 0 when Tabs has contents");
            compare(tabs.selectedTab, tab1, "The default selectedTab is the first tab");
            compare(tabs.currentPage, page1, "The default currentPage is the page of the first tab");
            compare(mainView.__propagated.toolbar.tools, page1.tools, "The default tools are the tools of the first tab");
            compare(mainView.__propagated.header.contents, tabs.tabBar, "Tabs updates the Header contents");
        }

        function test_tabsSetSelectedTab() {
            var tabArray = [tab1, tab2, tab3];
            var pageArray = [page1, page2, page3];
            for (var i=0; i < 3; i++) {
                tabs.selectedTabIndex = i;
                compare(tabs.selectedTabIndex, i, "Can set selectedTabIndex");
                compare(tabs.selectedTab, tabArray[i], "Can update selectedTab by setting selectedTabIndex");
                compare(tabs.currentPage, pageArray[i], "Can update currentPage by setting selectedTabIndex");
                compare(mainView.__propagated.toolbar.tools, pageArray[i].tools, "Activating a Tab updates the tools of the Toolbar");
                for (var j=0; j < 3; j++) {
                    compare(pageArray[j].active, j===i, "Only the page of the selected tab is active");
                }
            }
        }

        function test_flickable() {
            // ensure that the flickable of the header is set to the flickable of the selected tab.
            tabs.selectedTabIndex = 3;
            compare(mainView.__propagated.header.flickable, flickable1, "Header flickable correctly initialized");
            tabs.selectedTabIndex = 4;
            compare(mainView.__propagated.header.flickable, flickable2, "Header flickable correctly updated");
            tabs.selectedTabIndex = 0;
        }

        function test_pageLoader() {
            tabs.selectedTabIndex = 0;
            compare(loader.item, null, "Page not loaded when tab is not selected");
            tabs.selectedTabIndex = 5;
            compare(tabs.currentPage, loader, "Selected loader for current page");
            compare(loader.item.title, "Loaded page", "Loaded item is a page");
            tabs.selectedTabIndex = 0;
            compare(loader.item, null, "Loaded page was properly unloaded");
        }

        function test_bug1088740() {
            tabs.selectedTabIndex = 5;
            compare(mainView.__propagated.header.flickable, loader.item.flick, "Header flickable correctly updated with Loader");
            compare(loader.item.flick.contentHeight, 1000, "Header flickable is correct flickable");
            tabs.selectedTabIndex = 0;
        }

        function test_index() {
            compare(tab1.index, 0, "tab1 is at 0");
            compare(tab2.index, 1, "tab2 is at 1");
            compare(tab3.index, 2, "tab3 is at 2");
            compare(tabFlick1.index, 3, "tabFlick1 is at 3");
            compare(tabFlick2.index, 4, "tabFlick2 is at 4");
            compare(tabFlickLoader.index, 5, "tabFlickLoader is at 5");
        }

        function test_deactivateByTimeout() {
            tabs.tabBar.selectionMode = true;
            compare(tabs.tabBar.selectionMode, true, "Tab bar can be put into selection mode");
            compare(tabs.tabBar.__styleInstance.deactivateTime > 0, true, "There is a positive deactivate time");
            wait(tabs.tabBar.__styleInstance.deactivateTime + 500); // add 500 ms margin
            compare(tabs.tabBar.selectionMode, false, "Tab bar automatically leaves selection mode after a timeout.");
        }

        function test_deactivateByAppInteraction() {
            tabs.tabBar.selectionMode = true;
            compare(tabs.tabBar.selectionMode, true, "Tab bar can be put into selection mode");
            mouseClick(button, units.gu(1), units.gu(1), Qt.LeftButton);
            compare(tabs.tabBar.selectionMode, false, "Tab bar deactivated by interacting with the page contents");
        }

        function test_tabBar_pressed() {
            compare(tabs.tabBar.pressed, false, "Before user interaction, pressed is false");
            mousePress(tabs.tabBar, tabs.tabBar.width/2, tabs.tabBar.height/2);
            compare(tabs.tabBar.pressed, true, "Pressing the tab bar makes pressed true");
            mouseRelease(tabs.tabBar, tabs.tabBar.width/2, tabs.tabBar.height/2);
            compare(tabs.tabBar.pressed, false, "After releasing, pressed is false");
        }



        // these tests should not be mixed with Repeaters
        function test_z_addTab() {
            var newTab = tabs.addTab("Dynamic Tab", dynamicTab);
            compare((newTab !== null), true, "tab added");
            compare(newTab.active, false, "the inserted tab is inactive");
            compare(newTab.index, tabs.count - 1, "the tab is the last one");
        }

        function test_z_addExternalTab() {
            var newTab = tabs.addTab("External Tab", Qt.resolvedUrl("ExternalTab.qml"));
            compare((newTab !== null), true, "tab added");
            compare(newTab.active, false, "the inserted tab is inactive");
            compare(newTab.index, tabs.count - 1, "the tab is the last one");
        }

        function test_z_addTabWithDefaultTitle() {
            var newTab = tabs.addTab("", dynamicTab);
            compare((newTab !== null), true, "tab added");
            compare(newTab.title, "OriginalTitle", "tab created with original title");
        }

        function test_z_insertTab() {
            var tabIndex = Math.ceil(tabs.count / 2);
            var newTab = tabs.insertTab(tabIndex, "Inserted tab", dynamicTab);
            compare((newTab !== null), true, "tab inserted");
            compare(newTab.index, tabIndex, "this is the first tab");
            compare(tabs.selectedTab !== newTab, true, "the new tab is not the active one");
        }

        function test_z_insertExternalTab() {
            var tabIndex = Math.ceil(tabs.count / 2);
            var newTab = tabs.insertTab(tabIndex, "Inserted External tab", Qt.resolvedUrl("ExternalTab.qml"));
            compare((newTab !== null), true, "tab inserted");
            compare(newTab.index, tabIndex, "this is the first tab");
            compare(tabs.selectedTab !== newTab, true, "the new tab is not the active one");
        }

        function test_z_insertTabAtSelectedIndex() {
            tabs.selectedTabIndex = 1;
            var tabIndex = tabs.selectedTabIndex - 1;
            var newTab = tabs.insertTab(tabIndex, "InsertedAtSelected tab", dynamicTab);
            compare((newTab !== null), true, "tab inserted");
            compare(newTab.index, tabIndex, "inserted at selected tab");
            compare(tabs.selectedTabIndex != (tabIndex + 1), true, "it is not the selected tab");
        }

        function test_z_insertTabFront() {
            var newTab = tabs.insertTab(-1, "PreTab", dynamicTab);
            compare(newTab !== null, true, "pre-tab inserted");
            compare(newTab.index, 0, "this is the new first tab");
            compare(tabs.selectedTab !== newTab, true, "the new tab is not the active one");
        }

        function test_z_insertTabEnd() {
            var newTab = tabs.insertTab(tabs.count, "PostTab", dynamicTab);
            compare(newTab !== null, true, "post-tab inserted");
            compare(newTab.index, tabs.count - 1, "thsi is the new last tab");
            compare(tabs.selectedTab !== newTab, true, "the new tab is not the active one");
        }

        function test_z_insertTabAndActivate() {
            var newTab = tabs.addTab("Inserted tab", dynamicTab);
            compare((newTab !== null), true, "tab inserted");
            compare(newTab.index, tabs.count - 1, "the tab is the last one");
            tabs.selectedTabIndex = newTab.index;
            compare(tabs.selectedTab, newTab, "the inserted tab is selected");
            compare(newTab.active, true, "the new tab is active");
        }

        function test_z_moveTab() {
            var selectedIndex = tabs.count - 1;
            tabs.selectedTabIndex = selectedIndex;
            compare(tabs.moveTab(0, selectedIndex), true, "first tab moved to last");
            compare(tabs.selectedTabIndex, selectedIndex - 1, "the selected index moved backwards");
            tabs.selectedTabIndex = selectedIndex = 0;
            compare(tabs.moveTab(selectedIndex, selectedIndex + 1), true, "selected tab moved as next");
            compare(tabs.selectedTabIndex, selectedIndex + 1, "the selected index moved forewards");
        }

        function test_z_moveSelectedTab() {
            tabs.selectedTabIndex = 0;
            tabs.moveTab(0, 1);
            compare(tabs.selectedTabIndex, 1, "selected tab moved");
        }

        function test_z_moveTabFail() {
            compare(tabs.moveTab(-1, tabs.count - 1), false, "from-parameter out of range");
            compare(tabs.moveTab(0, tabs.count), false, "to-parameter out of range");
        }

        function test_z_removeTab() {
            compare(tabs.removeTab(tabs.count - 1), true, "last tab removed");
            tabs.selectedTabIndex = 0;
            compare(tabs.removeTab(0), true, "active tab removed");
            compare(tabs.selectedTabIndex, 0, "the next tab is selected")
        }

        function test_z_removeActiveTab() {
            tabs.selectedTabIndex = 1;
            compare(tabs.removeTab(1), true, "selected tab removed");
            compare(tabs.selectedTabIndex, 1, "selected tab is next");

            tabs.selectedTabIndex = tabs.count - 1;
            compare(tabs.removeTab(tabs.count - 1), true, "last tab removed");
            compare(tabs.selectedTabIndex, tabs.count - 1, "selected tab moved to last item");
        }

        function test_z_removeTabAfterActiveTab() {
            var activeTab = tabs.count - 2;
            tabs.selectedTabIndex = activeTab;
            compare(tabs.removeTab(tabs.count - 1), true, "last tab removed");
            compare(tabs.selectedTabIndex, activeTab, "the selected tab wasn't moved");
        }

        function test_z_removeTabBeforeActiveTab() {
            var activeTab = tabs.count - 1;
            tabs.selectedTabIndex = activeTab;
            compare(tabs.removeTab(0), true, "first tab removed");
            compare(tabs.selectedTabIndex, activeTab - 1, "the selected tab index decreased");
        }

        function test_zz_addTabAfterCleaningUpTabs() {
            while (tabs.count > 1) {
                tabs.removeTab(tabs.count - 1);
            }
            compare(tabs.selectedTabIndex, 0, "the only tab is the selected one");
            // add a new tab anc check the count (default added tas should not be added anymore
            tabs.addTab("Second tab", dynamicTab);
            compare(tabs.count, 2, "we have two tabs only");
        }

        function test_zz_addPredeclaredTab() {
            tabs.removeTab(tab1.index);

            // add a predeclared tab back with original title
            compare(tabs.addTab("", tab1), tab1, "tab1 was not added back");
            compare(tab1.title, "tab 1", "the original title differs");

            // add a predeclared tab which was added already
            compare(tabs.addTab("", tab1), null, "tab1 is already in tabs");
        }
    }
}

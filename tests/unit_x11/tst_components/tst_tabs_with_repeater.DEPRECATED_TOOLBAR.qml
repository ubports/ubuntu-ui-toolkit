/*
 * Copyright 2014 Canonical Ltd.
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

import QtQuick 2.2
import Ubuntu.Components 1.1
import Ubuntu.Test 1.0

MainView {
    id: testCase
    width: units.gu(50)
    height: units.gu(100)

    useDeprecatedToolbar: true

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

    UbuntuTestCase {
        name: "TabsWithRepeaterDeprecatedToolbar"
        when: windowShown

        /*
          The following testcases are all related to bug #1253804
          */
        function test_tabOrder_bug1253804() {
            var tabsModel = tabsWithRepeater.__model;

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
            compare(tabsWithRepeater.__model.count, 0, "There are still tabs left after repeater model is reset");
        }

        function test_repeaterTabs() {
            repeater.model = inputModel;
            var tabsModel = repeaterTabs.__model;

            compare(repeater.count, inputModel.count, "Incorrect number of tabs in Tabs");
            compare(tabsModel.count, repeater.count, "Incorrect number of tabs in TabBar");
            for (var i=0; i < tabsModel.count; i++) {
                compare(tabsModel.get(i).title, inputModel.get(i).name, "Tab titles don't match for index "+i);
            }

            // clear repeaterTabs
            repeater.model = null;
            compare(repeaterTabs.__model.count, 0, "There are still tabs left after repeater model is reset");
        }

        function test_repeaterTabs_arrayAsModel() {
            repeater.model = testCase.listModel;
            var tabsModel = repeaterTabs.__model;

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
            compare(repeaterTabs.__model.count, 0, "There are still tabs left after repeater model is reset");

        }

        function test_twoRepeaters() {
            var tabsModel = twoRepeaters.__model;
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
            var tabsModel = twinRepeaters.__model;

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

            /* FIXME
            for (var j = 0; j < 2; j++) {
                for (var i = 0; i < twinModel.count; i++) {
                    var index = j * twinModel.count + i;
                    compare(tabsModel.get(index).title, twinModel.get(i).name, "Tab titles don't match for Tabs index " + index);
                }
            }
            */

            // set it to null
            twinRepeater1.model = null;
            twinRepeater2.model = null;
            compare(twinRepeaters.__model.count, 0, "There are still tabs left after repeater model is reset");
        }
    }
}

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

Item {
    width: units.gu(40)
    height: units.gu(71)

    Loader {
        id: testLoader
        anchors.fill: parent
    }

    UbuntuTestCase {
        name: "SubTheming"
        when: windowShown

        property string suru: "Ubuntu.Components.Themes.SuruDark"
        property string ambiance: "Ubuntu.Components.Themes.Ambiance"

        SignalSpy {
            id: parentThemeChangeSpy
            signalName: "parentThemeChanged"
        }
        SignalSpy {
            id: themeChangeSpy
            signalName: "themeChanged"
        }

        function cleanup() {
            testLoader.source = "";
            parentThemeChangeSpy.target = null;
            parentThemeChangeSpy.clear();
            themeChangeSpy.target = null;
            themeChangeSpy.clear();
        }

        function getTest(test) {
            testLoader.source = Qt.resolvedUrl("subtheming/" + test);
            tryCompareFunction(function() { return testLoader.status == Loader.Ready }, true, 1000);
            return testLoader.item;
        }

        function test_parent_change_when_assinged() {
            var main = getTest("DynamicAssignment.qml");
            var testSet = findInvisibleChild(main, "testSet");
            var testItem = findChild(main, "testItem");
            verify(testSet);
            parentThemeChangeSpy.target = testSet;
            testItem.theme = testSet;
            parentThemeChangeSpy.wait();
            compare(testSet.parentTheme, main.theme, "Themes differ");
        }

        function test_parent_set_reset_triggers_parent_change() {
            var main = getTest("ParentChanges.qml");
            var testItem = findChild(main, "testItem");
            var testSet = testItem.theme;
            verify(testSet);
            parentThemeChangeSpy.target = testSet;
            main.theme = undefined;
            parentThemeChangeSpy.wait();
            compare(testSet.parentTheme, main.theme, "Themes differ");
        }

        function test_parent_set_namechange_triggers_parent_change() {
            var main = getTest("ParentChanges.qml");
            var testItem = findChild(main, "testItem");
            var testSet = testItem.theme;
            verify(testSet);
            parentThemeChangeSpy.target = testSet;
            main.theme.name = "Ubuntu.Components.Themes.SuruDark";
            parentThemeChangeSpy.wait();
            compare(testSet.parentTheme, main.theme, "Themes differ");
        }

        function test_theme_change_data() {
            return [
                        {tag: "Suru on mainStyled",
                            themeName: suru, applyOnItem: "mainStyled",
                            testItems: ["firstLevelStyled", "secondLevelStyled", "firstLevelLoaderStyled"],
                            expectedStyleNames: [suru, suru, suru],
                            sameTheme: [true, true, true]
                        },
                        {tag: "Suru on firstLevelStyled",
                            themeName: suru, applyOnItem: "firstLevelStyled",
                            testItems: ["secondLevelStyled", "firstLevelLoaderStyled"],
                            expectedStyleNames: [suru, ambiance],
                            sameTheme: [true, false]
                        },
                    ]
        }
        function test_theme_change(data) {
            var validTest = data.testItems.length == data.expectedStyleNames.length && data.testItems.length == data.sameTheme.length;
            verify(validTest, "testItems, expectedStyleNames, and sametheme must have same amount of values");

            var main = getTest("TestStyleChange.qml");
            var theme = findInvisibleChild(main, "Theme");
            var styledItem = findChild(main, data.applyOnItem);
            verify(styledItem.theme != theme, "ThemeSettings should not be set yet!");
            theme.name = data.themeName;
            // set the style on the item
            styledItem.theme = theme;
            waitForRendering(main, 400);
            for (var i = 0; i < data.testItems.length; i++) {
                var itemName = data.testItems[i];
                var themeName = data.expectedStyleNames[i];
                var success = data.sameTheme[i];
                styledItem = findChild(main, itemName);
                compare(styledItem.theme == theme, success);
                compare(styledItem.theme.name, themeName);
            }
        }

        function test_reparent_styled_data() {
            return [
                {tag: "Move item out of Suru sub-themed",
                            suruStyledItem: "firstLevelStyled", testStyledItem: "secondLevelStyled",
                            testStyledItemThemes: [ambiance, suru, ambiance],
                            sourceItemName: "secondLevelStyled", newParentItemName: "firstLevelLoader"
                },
                {tag: "Move item into Suru sub-themed",
                            suruStyledItem: "firstLevelStyled", testStyledItem: "firstLevelLoaderStyled",
                            testStyledItemThemes: [ambiance, ambiance, suru],
                            sourceItemName: "firstLevelLoader", newParentItemName: "secondLevelStyled"
                },
                {tag: "Move dynamic item out of Suru sub-themed",
                            suruStyledItem: "firstLevelStyled", testStyledItem: "thirdLevelLoaderStyled",
                            testStyledItemThemes: [ambiance, suru, ambiance],
                            sourceItemName: "secondLevelStyled", newParentItemName: "firstLevelLoader"
                },
            ]
        }
        function test_reparent_styled(data) {
            var main = getTest("TestStyleChange.qml");
            var theme = findInvisibleChild(main, "Theme");
            var testItem = findChild(main, data.testStyledItem);
            var suruItem = findChild(main, data.suruStyledItem);
            var sourceItem = findChild(main, data.sourceItemName);
            var newParentItem = findChild(main, data.newParentItemName);

            compare(testItem.theme.name, data.testStyledItemThemes[0], "test styled item theme differs");

            theme.name = "Ubuntu.Components.Themes.SuruDark";
            suruItem.theme = theme;
            waitForRendering(main, 500);
            compare(testItem.theme.name, data.testStyledItemThemes[1], "changed theme differs");

            //reparent
            verify(sourceItem != newParentItem);
            verify(sourceItem.parent != newParentItem);
            themeChangeSpy.target = testItem;
            sourceItem.parent = newParentItem;
            themeChangeSpy.wait();
            compare(testItem.theme.name, data.testStyledItemThemes[2], "reparented theme differs");
        }
    }
}

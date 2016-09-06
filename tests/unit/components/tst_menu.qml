/*
 * Copyright 2016 Canonical Ltd.
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
import Ubuntu.Components 1.3
import Ubuntu.Components.Labs 1.0

TestCase {
    name: "MenuAPI"

    TestUtil {
        id: util
    }

    function test_menubar() {
        compare(menuBar.menus.length, 2, "Incorrect number of menus in menubar")
        compare(menuBar.menus[0], submenu1, "Incorrect element found at menu bar index")
        compare(menuBar.menus[1], submenu2, "Incorrect element found at menu bar index")
    }

    function test_menu() {
        compare(floatingMenu.data.length, 5, "Incorrect number of menu items in menu");
        compare(floatingMenu.data[0], actionList, "Incorrect element found at menu index");
        compare(floatingMenu.data[1], menuGroup, "Incorrect element found at menu index");
        compare(floatingMenu.data[2], action1, "Incorrect element found at menu index");
        compare(floatingMenu.data[3], action2, "Incorrect element found at menu index");
        compare(floatingMenu.data[4], menu1, "Incorrect element found at menu index");
    }

    function test_menugroup() {
        compare(group.data.length, 4, "Incorrect number of menu items in MenuGroup");
        compare(group.data[0], groupAction, "Action not found at correct index in MenuGroup");
        compare(group.data[1], groupList, "ActionList not found at correct index in MenuGroup");
        compare(group.data[2], groupMenu, "Menu not found at correct index in MenuGroup");
        compare(group.data[3], groupGroup, "MenuGroup not found at correct index in MenuGroup");
    }

    function test_dynamic_append() {
        dynamicMenu.appendObject(floatingAction);
        compare(dynamicMenu.data.length, 1, "Action not added to menu");

        dynamicMenu.removeObject(floatingAction);
        compare(dynamicMenu.data.length, 0, "Action not removed from menu");
    }

    function test_dynamic_action_list_append() {
        dynamicMenu.appendObject(floatingList);
        compare(dynamicMenu.data.length, 1, "ActionList not added to menu");

        dynamicMenu.removeObject(floatingList);
        compare(dynamicMenu.data.length, 0, "ActionList not removed from menu");
    }

    function test_dynamic_insert() {
        dynamicMenu.insertObject(0, floatingAction);
        dynamicMenu.insertObject(0, floatingList);
        compare(dynamicMenu.data.length, 2, "ActionList not removed from menu");

        compare(dynamicMenu.data[0], floatingList, "Object was not inserted at correct index");
        compare(dynamicMenu.data[1], floatingAction, "Object was not inserted at correct index");

        dynamicMenu.removeObject(floatingAction);
        dynamicMenu.removeObject(floatingList);
        compare(dynamicMenu.data.length, 0, "Menu should be empty");
    }

    MenuBar {
        id: menuBar
        Menu {
            id: submenu1
        }
        Menu {
            id: submenu2
        }
    }

    Menu {
        id: floatingMenu
        ActionList {
            id: actionList
        }
        MenuGroup {
            id: menuGroup
        }
        Action {
            id: action1
        }
        Action {
            id: action2
        }
        Menu {
            id: menu1
        }
    }

    MenuGroup {
        id: group
        Action {
            id: groupAction
        }
        ActionList {
            id: groupList
        }
        Menu {
            id: groupMenu
        }
        MenuGroup {
            id: groupGroup
        }
    }

    Menu {
        id: dynamicMenu
    }

    Action {
        id: floatingAction
    }

    ActionList {
        id: floatingList
    }
}

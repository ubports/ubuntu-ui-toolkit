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

//.pragma library

// TODO: can I make this variable private?
var tabs = []
var selectedIndex = -1 // -1 means no tab is selected.
var pageItem;

// empty the tabs
function clear() {
    tabs = [];
    print("children cleared");
}

function isTab(item) {
    // TODO: check for another type. what if no page is defined?
    if ("page" in item) return true;
    return (item && item.hasOwnProperty("page"));
}

// if isTab(item), add item to the list of tabs
function add(item) {
    if (isTab(item)) {
        tabs.push(item);
    }
}

// prints an overview of all tabs
function printAll() {
    print("printing " +tabs.length+" tabs.");
    for (var i=0; i < tabs.length; i++) {
        var text = tabs[i].text;
        print("tab " + i + ", text: "+text);
    } // for i
}

// Return the number of tabs
function num() {
    return tabs.length;
}

// Return the tab item
function get(i) {
    print("returning tab "+i+": "+typeof(tabs[i]));
    return tabs[i];
}

// Make tab i the selected tab.
function select(i) {
    print("selecting tab " +i);
    selectedIndex = i;
    for (var i=0; i < tabs.length; i++) {
       // tabs[i].visible = (selectedIndex == i);
    } // for i
}

function getSelectedIndex() {
    return selectedIndex;
}

function isSelected(index) {
    return (index == selectedIndex);
}

// Specify the QML Item that will contain the displayed page.
function setPageItem(item) {
    pageItem = item;
    return true;
}

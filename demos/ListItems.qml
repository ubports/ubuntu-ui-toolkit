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

import QtQuick 1.1
import "../Components"
import "../Components/ListItems" as ListItem

Rectangle {
    id: window

    width: 400
    height: 700

    color: "#e6e6e6"

    ListItem.Container {
        id: listitems
        anchors.fill: parent

        ListItem.Default {
            text: "Fiennes"
            iconSource: "avatar_contacts_list.png"
            selected: false
            onClicked: selected = !selected
        }
        ListItem.Default {
            text: "Disabled"
            iconSource: "avatar_contacts_list.png"
            onClicked: print("Clicked disabled list item.") // doesn't happen
            enabled: false
        }
        ListItem.Default {
            text: "very long list item text that will be elided"
        }
        ListItem.Default {
            text: "very long list item text that will be elided"
            progression: true
        }
        ListItem.SingleValue {
            text: "Gambon"
            iconSource: "avatar_contacts_list.png"
            value: "Status"
            progression: true
            onClicked: print("clicked Gambon")
        }
        ListItem.Default {
            text: "Caine"
            iconSource: "avatar_contacts_list.png"
            progression: true
            onClicked: print("clicked Caine")
        }
        ListItem.MultiValue {
            text: "Label"
            values: ["Value 1", "Value 2", "Value 3", "Value 4"]
            onClicked: selected = !selected
        }
        ListItem.MultiValue {
            text: "Label"
            values: ["Value 1", "Value 2", "Value 3", "Value 4"]
            progression: true
        }
        ListItem.MultiValue {
            text: "Label"
            iconSource: "avatar_contacts_list.png"
            values: "progression"
            progression: true
            onClicked: print("YEAH!")
        }
    } // Column
} // window

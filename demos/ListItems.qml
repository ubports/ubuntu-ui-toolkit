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

    Column {
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
            onClicked: { print("Clicked disabled list item."); selected = !selected; }
            enabled: false
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
//        ListItem.SingleValue {
//            text: "Parameter"
//            value: "Status"
//        }
//        ListItem.SingleValue {
//            text: "Parameter Split"
//            value: "Status"
//            progression: true
//        }
//        ListItem.Default {
//            text: "Control"
//            control: Button {
//                text: "Button"
//                onClicked: print("click!")
//                height: 30
//            }
//        }
//        ListItem.Default {
//            text: "Control Split"
//            control: Button {
//                text: "Click"
//                onClicked: print("Button clicked")
//                height: 30
//            }
//            progression: true
//            onClicked: print("Progress clicked")
//        }
//        ListItem.Selector {
//            text: "Selector"
//            iconSource: "avatar_contacts_list.png"
//            control: TextCustom { text: "TODO"; color: "red" }
//            progression: false
//        }
//        ListItem.MultiValue {
//            text: "Label"
//            subText: "Value 1, Value 2, Value 3, Value 4"
//        }
//        ListItem.MultiValue {
//            text: "Label"
//            subText: "Value 1, Value 2, Value 3, Value 4"
//            progression: true
//        }
//        ListItem.Default {
//            text: "Action"
//            subText: "Button ---->"
//            control: Button { text: "click me" }
//        }
        ListItem.Default {
            text: "very very very loooong text, doesn't even fit"
        }

//        ListItem.Default { // Subtitled?
//            text: "Split"
//            subText: "action and progress"
//            control: Button { text: "button" }
//            progression: true
//        }
//        ListItem.Default { // not yet supported
//            text: "Multiple lines subtitle"
//            subText: "The secondary label can have up to\nthree lines of text,\nand the listitem should scale automatically?"
//        }
    } // Column
} // window

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
import  "../Components"

Rectangle {
    id: window

    width: 400
    height: 700

    Column {
        id: listitems
        anchors.fill: parent

        ListItem {
            text: "Basic"
            onClicked: print("Basic button clicked.")
        }
        ListItem {
            text: "Icon"
            iconSource: "call_icon.png"
        }
        ListItem {
            text: "Progression"
            progression: true
            onClicked: print("Progressing...")
        }
        ListItem {
            text: "Single value"
            control: TextCustom {
                text: "Value 1"
                elide: Text.ElideRight
            }
            progression: true
        }
        ListItem {
            text: "Control"
            control: Button {
                text: "Button"
                onClicked: print("click!")
                height: 30
            }
        }
        ListItem {
            text: "Split"
            control: Button {
                text: "Click"
                onClicked: print("Button clicked")
                height: 30
            }
            progression: true
            onClicked: print("Progress clicked")
        }
        ListItem {
            text: "Selector"
            control: TextCustom { text: "TODO"; color: "red" }
            progression: false
        }
        SubtitledListItem {
            text: "Subtitle"
            subtext: "Secondary label"
        }
        SubtitledListItem {
            text: "Multiple lines subtitle"
            subtext: "The secondary label can have up to\nthree lines of text,\nand the listitem should scale automatically?"
        }
    } // Column
} // window

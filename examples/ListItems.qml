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
    height: 600

    Column {
        id: listitems
        anchors {
            fill: parent
        }
        //spacing: 5

        DefaultListItem {
            height: 60;
            width: parent.width;
            color: "blue"
            text: "blue"
            control: Button {
                width: 100
                height: 35
                text: "control"
                color: "pink"
                onClicked: print("clicked control")
            }
        }
        DefaultListItem {
            height: 60;
            width: parent.width;
            color: "yellow"
            text: "yellow"
            iconSource: "call_icon.png"
            progression: true
            onClicked: print("Progressing...")
        }
        DefaultListItem { height: 60;
            width: parent.width;
            color: "green"
            text: "green"
            progression: false
            onClicked: print("This won't happen")
        }
    } // Column
} // window

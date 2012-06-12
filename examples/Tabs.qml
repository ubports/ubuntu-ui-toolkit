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

Rectangle {
    id: window

    width: 800
    height: 600

    Row {
        id: buttonRow
        anchors.horizontalCenter: window.horizontalCenter
        TabButton {
            text: "Tab 1"
            tab: tab1
        }
        TabButton {
            text: "Tab 2a"
            tab: tab2
            width: 80
        }
        TabButton {
            text: "Tab 2b"
            tab: tab2
            width: 80
            // This button has the same tab as the previous TabButton.
            // When the tab is active, both buttons will appear selected.
        }
        TabButton {
            text: "Tab 3"
            tab: tab3
        }
        TabButton {
            text: "No tab"
            // no associated tab. The button will be visible
            // but does not select a tab when clicked.
        }
        TabButton {
            text: "Invalid"
            tab: notATab
            // no valid tab. The button will be visible
            // but does not select a tab when clicked.
        }
    } // Row

    TabGroup {
       currentTab: tab1
       anchors.top: buttonRow.bottom
       anchors.horizontalCenter: window.horizontalCenter

       Item {
           id: tab1
           anchors.fill: parent
           Text {
               anchors.centerIn: parent
               text: "This is the first tab."
           }
       }
       Item {
           id: tab2
           anchors.fill: parent
           Text {
               anchors.centerIn: parent
               text: "Tab number two."
           }
       }
       Rectangle {
           id: tab3
           anchors.fill: parent
           Text {
               anchors.centerIn: parent
               text: "Colorful tab 3"
           }
           color: "lightblue"
       }
   }
   Item {
       // this Item is not a valid tab because its
       // parent is not a TabGroup.
       id: notATab
   }
} // window

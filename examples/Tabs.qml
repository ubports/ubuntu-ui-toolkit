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

    color: "#eeeeee"

    Tabs {
        id: tabs
        anchors.fill: parent
        buttonsExpanded: true
        selectedTabIndex: 0
        Tab {
            text: "tab 1"
            iconSource: "call_icon.png"
            page: Text {
                anchors.centerIn: parent
                text: "This is the first tab."
            }
        }
        Tab {
            text: "buttons!"
            iconSource: "call_icon.png"
            pageSource: "Buttons.qml"
            pagePreloaded: false
        }
        Tab {
            id: tab3
            text: "tab 3"
            page:  Rectangle {
                anchors.fill: parent
                Text {
                    anchors.centerIn: parent
                    text: "Colorful tab 3"
                }
                color: "lightblue"
            }
            iconSource: tab3.selected ? "call_icon.png" : ""
        }
    } // Tabs
} // window

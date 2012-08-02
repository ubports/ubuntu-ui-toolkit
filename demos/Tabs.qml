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
import Ubuntu.Components 0.1

Template {
    title: "Tabs"

    Tabs {
        id: tabs
        property variant numerations: ["first", "second", "third"]
        property variant colors: ["#eeeeee", "#e4e4e4", "#dddddd"]
        property variant showIcon: [false, true, true]
        property variant showTitle: [true, false, true]

        Repeater {
            model: tabs.numerations
            Page {
                title: tabs.showTitle[index] ? "Tab "+ (index + 1) : ""
                iconSource: tabs.showIcon[index] ? "call_icon.png" : ""
                contents: Rectangle {
                    color: tabs.colors[index]
                    anchors.fill: parent
                    TextCustom {
                        anchors.centerIn: parent
                        text: "This is the " + modelData + " tab."
                        color: "#757373"
                    }
                }
            }
        }
    }
}

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
        Page {
            text: "Tab 1"
            page: Rectangle {
                color: "#eeeeee"
                anchors.fill: parent
                TextCustom {
                    anchors.centerIn: parent
                    text: "This is the first tab."
                    color: "#757373"
                }
            }
        }
        Page {
            iconSource: "call_icon.png"
            page: Rectangle {
                color: "#e4e4e4"
                anchors.fill: parent
                TextCustom {
                    anchors.centerIn: parent
                    text: "This is the second tab."
                    color: "#757373"
                }
            }
        }
        Page {
            text: "Tab 3"
            iconSource: "call_icon.png"
            page:  Rectangle {
                color: "#dddddd"
                anchors.fill: parent
                TextCustom {
                    anchors.centerIn: parent
                    text: "This is the third tab."
                    color: "#757373"
                }
            }
        }
    }
}

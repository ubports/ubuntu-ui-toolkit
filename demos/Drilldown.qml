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
    title: "Drilldown"

    PageStack {
        id: pageStack
//        anchors.fill: parent
        showToolBar: true
        width: 250
        anchors {
            top: parent.top
            left: parent.left
            bottom: parent.bottom
        }

        Drilldown {
            title: "Colors"
            Drilldown {
                title: "Dark"
                Page {
                    title: "BLACK"
                    contents: Rectangle { anchors.fill: parent; color: "#111111" }
                }
            }
            Drilldown {
                title: "Bright"
                Page {
                    title: "Orange"
                    contents: Rectangle { anchors.fill: parent; color: "orange" }
                }
                Drilldown {
                    id: blues
                    title: "Blueish"
                    property variant names: ["aqua", "navy",
                        "steelblue", "lightsteelblue", "cornflowerblue", "turquoise",
                        "midnightblue", "slateblue", "royalblue", "powderblue"]

                    Repeater {
                        model: blues.names
                        Page {
                            title: modelData
                            contents: Rectangle { anchors.fill: parent; color: modelData }
                        }
                    }
                }
                Page {
                    title: "Green"
                    contents: Rectangle { anchors.fill: parent; color: "green" }
                }
            }
        }
    }
}

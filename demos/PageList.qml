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
import Ubuntu.Components.ListItems 0.1
Template {
    title: "Drilldown"

    PageStack {
        id: pageStack
        anchors.fill: parent

        Page {
            id: firstPage
            title: "Colors"
            contents: Column {
                anchors.fill: parent

                PageList {
                    title: "Light"
                    pageStack: pageStack

                    property variant colorList: ["white", "yellow", "pink", "lightgrey"]
                    Repeater {
                        model: parent.colorList
                        Page {
                            title: modelData
                            contents: Rectangle { anchors.fill: parent; color: modelData }
                        }
                    }
                }
                PageList {
                    title: "Dark"
                    pageStack: pageStack

                    Page {
                        title: "brown"
                        contents: Rectangle { anchors.fill: parent; color: "brown" }
                    }
                    Page {
                        title: "blue"
                        contents: PageList {
                            pageStack: pageStack
                            Page {
                                title: "Navy"
                                contents: Rectangle { anchors.fill: parent; color: "navy" }
                            }
                            Page {
                                title: "Steelblue"
                                contents: Rectangle { anchors.fill: parent; color: "steelblue" }
                            }
                            Page {
                                title: "sky"
                                contents: Rectangle { anchors.fill: parent; color: "skyblue" }
                            }
                        }
                    }
                    Page {
                        title: "black"
                        contents: Rectangle { anchors.fill: parent; color: "black" }
                    }
                }
            }
        }
    }
}

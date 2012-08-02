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
        anchors.fill: parent
        showToolBar: true

        rootPage: Drilldown {
            title: "Colors"
            //anchors.fill: parent
            __pageStack: pageStack
            Drilldown {
                title: "Dark"
                __pageStack: pageStack
                Page {
                    title: "BLACK"
                    contents: Rectangle { anchors.fill: parent; color: "#111111" }
                }
            }
            Drilldown {
                title: "Bright"
                __pageStack: pageStack
                Page {
                    title: "Orange"
                    contents: Rectangle { anchors.fill: parent; color: "orange" }
                }
                Page {
                    title: "Blue"
                    contents: Rectangle { anchors.fill: parent; color: "blue" }
                }
                Page {
                    title: "Green"
                    contents: Rectangle { anchors.fill: parent; color: "green" }
                }
            }
        }
    }
}

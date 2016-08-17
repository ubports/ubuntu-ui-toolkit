/*
 * Copyright 2016 Canonical Ltd.
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

import QtQuick 2.4
import Ubuntu.Components 1.3

MainView {
    width: units.gu(48)
    height: units.gu(60)
    objectName: "mainView"
    Page {
        id: page
        header: PageHeader {
            title: "Sections test"
        }
        Label {
            id: label
            objectName: "label"
            anchors {
                top: page.header.bottom
                topMargin: units.gu(2)
                horizontalCenter: parent.horizontalCenter
            }
            text: "Section " + sections.selectedIndex + " is selected."
        }
        Sections {
            id: sections
            objectName: "sections"
            anchors {
                horizontalCenter: parent.horizontalCenter
                top: label.bottom
                topMargin: units.gu(4)
            }
            model: [ "first", "second", "third" ]
        }
        Label {
            id: moreLabel
            objectName: "scrolling_label"
            anchors {
                centerIn: parent
            }
            text: "Scrollable section " + moreSections.selectedIndex + " is selected."
        }
        Sections {
            id: moreSections
            objectName: "scrolling_sections"
            anchors {
                horizontalCenter: parent.horizontalCenter
                top: moreLabel.bottom
                topMargin: units.gu(4)
                left: parent.left
                right: parent.right
            }
            model: ["one", "two", "three", "four", "five", "six",
                "seven", "eight", "nine", "ten", "eleven", "twelve",
                "thirteen", "fourteen", "fifteen", "sixteen",
                "seventeen", "eighteen", "nineteen", "twenty"
            ]
        }
    }
}

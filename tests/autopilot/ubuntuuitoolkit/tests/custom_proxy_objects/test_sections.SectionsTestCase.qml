/*
 * Copyright 2015 Canonical Ltd.
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
        title: "Sections test"
        Label {
            id: label
            objectName: "label"
            anchors {
                top: parent.top
                horizontalCenter: parent.horizontalCenter
            }
            text: "Section " + sections.selectedIndex + " is selected."
        }
        Sections {
            id: sections
            objectName: "sections"
            anchors.centerIn: parent
            model: [ "first", "second", "third" ]
        }
    }
}

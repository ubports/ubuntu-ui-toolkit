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

import QtQuick 2.0
import Ubuntu.Components 1.1


MainView {
    id: view
    width: 800
    height: 600

    ListModel {
        id: pages
        property int selectedIndex: 0
        ListElement {
            title: "Tab 1"
        }
        ListElement {
            title: "Tab 2"
        }
        ListElement {
            title: "Tab 3"
        }
        ListElement {
            title: "Tab 4"
        }
        ListElement {
            title: "Tab 5"
        }
    }

    Header {
        id: header
        tabsModel: pages
    }

    Component {
        id: tabComponent
        Rectangle {
            width: ListView.view.width
            height: ListView.view.height
            Label {
                anchors.centerIn: parent
                text: title
                fontSize: "x-large"
            }
            color: Qt.rgba(Math.random(0.5), Math.random(1), Math.random(0.5), 1)
            MouseArea {
                anchors.fill: parent
                onClicked: pages.selectedIndex = index
            }
        }
    }
    ListView {
        id: list
        anchors {
            top: header.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        orientation: Qt.Horizontal
        model: pages
        delegate: tabComponent
        currentIndex: pages.selectedIndex
    }
}

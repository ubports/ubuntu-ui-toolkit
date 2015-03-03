/*
 * Copyright (C) 2015 Canonical Ltd.
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
import Ubuntu.Components 1.2

MainView {
    id: mainView
    width: units.gu(50)
    height: units.gu(70)

    Page {
        id: flickingPage
        title: "Auto-hide"
        head {
            locked: lockedSwitch.checked
            onVisibleChanged: {
                visibleSwitch.checked = flickingPage.head.visible
            }
        }
        Flickable {
            anchors.fill: parent
            contentHeight: units.gu(200)
            Grid {
                id: switchGrid
                columns: 2
                spacing: units.gu(1)
                anchors {
                    top: parent.top
                    left: parent.left
                    margins: units.gu(6)
                }
                Switch {
                    id: lockedSwitch
                    checked: false
                }
                Label {
                    text: "header locked"
                }
                Switch {
                    id: visibleSwitch
                    checked: flickingPage.head.visible
                    onClicked: flickingPage.head.visible = checked
                }
                Label {
                    text: "header visible"
                }
            }
            Label {
                anchors {
                    horizontalCenter: parent.horizontalCenter
                    top: switchGrid.bottom
                    topMargin: units.gu(15)
                }
                text: "Please flick me"
            }
        }
    }
}

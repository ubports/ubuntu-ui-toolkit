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
        title: "ActionBar test"

        Label {
            id: label
            objectName: "Label"
            anchors {
                top: parent.top
                horizontalCenter: parent.horizontalCenter
            }
            text: "No action triggered."
        }

        ActionBar {
            objectName: "ActionBar"
            anchors.centerIn: parent
            numberOfSlots: 2
            actions: [
                Action {
                    iconName: "share"
                    text: "Share"
                    onTriggered: label.text = "Action 1 triggered."
                    objectName: "Action1"
                },
                Action {
                    iconName: "starred"
                    text: "Favorite"
                    onTriggered: label.text = "Action 2 triggered."
                    objectName: "Action2"
                },
                Action {
                    iconName: "lock"
                    text: "Lock"
                    onTriggered: label.text = "Action 3 triggered."
                    objectName: "Action3"
                }
            ]
        }
    }
}

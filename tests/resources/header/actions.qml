/*
 * Copyright (C) 2013-2015 Canonical Ltd.
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
    width: units.gu(50)
    height: units.gu(70)

    Page {
        title: "Actions"
        Label {
            anchors.centerIn: parent
            text: "New API"
        }
        head {
            actions: [
                Action {
                    iconName: "settings"
                    onTriggered: print("Trigger first action")
                },
                Action {
                    iconName: "camera-flip"
                    onTriggered: print("Trigger second action")
                }
            ]
            backAction: Action {
                iconName: "close"
                onTriggered: print("Trigger custom back action")
            }
        }
    }
}

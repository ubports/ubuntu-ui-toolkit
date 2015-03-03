/*
 * Copyright (C) 2014 Canonical Ltd.
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
    id: app
    width: units.gu(40)
    height: units.gu(71)
    applicationName: "StateSaver"

    StateSaver.properties: "width, height"

    Page {
        id: page
        title: "StateSaver test"

        Rectangle {
            id: rect
            anchors.fill: parent
            color: "red"

            StateSaver.properties: "color"

            Column {
                anchors.centerIn: parent
                Button {
                    text: app.StateSaver.enabled ? "Disable" : "Enable" + " StateSaver"
                    onClicked: {
                        app.StateSaver.enabled = !app.StateSaver.enabled
                    }
                }
                Button {
                    text: "Random color"
                    onClicked: {
                        rect.color = Qt.rgba(Math.random(1.0) * 1, Math.random(1.0) * 1, Math.random(1.0) * 1, 1)
                    }
                }
            }
        }
    }
}

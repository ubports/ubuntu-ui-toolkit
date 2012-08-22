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
    title: "Activity Indicator"
    id: indicators

    Column {
        id: column1
        spacing: 30

        Button {
            id: button1
            text: (indicator.running) ? "Deactivate" : "Activate"
            width: 170
            onClicked: indicator.running = !indicator.running
        }

        ActivityIndicator {
            id: indicator
            width: 80
            height: 80
            anchors.horizontalCenter: button1.horizontalCenter
        }
    }

    Component.onCompleted: indicator.running = true
}

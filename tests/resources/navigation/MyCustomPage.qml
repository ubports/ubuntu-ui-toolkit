/*
 * Copyright 2012-2014 Canonical Ltd.
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

import QtQuick 2.2
import Ubuntu.Components 1.3

Page {
    title: i18n.tr("My custom page")

    Flickable {
        anchors.fill: parent
        contentHeight: height + units.gu(10)
        Label {
            anchors {
                top: parent.top
                topMargin: units.gu(16)
                horizontalCenter: parent.horizontalCenter
            }

            text: i18n.tr("This is an external page.")
            color: "#757373"
        }
    }

    head.actions: [
        Action {
            text: "action 1"
            iconName: "outgoing-call"
        },
        Action {
            text: "action 2"
            iconName: "missed-call"
        },
        Action {
            text: "another one"
            iconSource: "call_icon.png"
            enabled: false
        }
    ]
}

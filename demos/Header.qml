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
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem

Template {
    title: i18n.tr("Header")
    objectName: "Header"

    Item {
        MainView {
            id: mainView
            anchors {
                top: parent.top
                horizontalCenter: parent.horizontalCenter
            }
            width: units.gu(38)
            height: units.gu(50)

            Page {
                anchors.fill: parent
                title: i18n.tr("Header demo")

                Label {
                    text: "Page contents"
                    anchors.centerIn: parent
                    // TODO: make this scrollable to test the header.
                }
            }
        }
    }
}

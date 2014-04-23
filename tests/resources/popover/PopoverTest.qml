/*
 * Copyright 2013 Canonical Ltd.
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
import Ubuntu.Components.Popups 1.0
import Ubuntu.Components.ListItems 1.0 as ListItem

MainView {
    width: units.gu(80)
    height: units.gu(34)

    Component {
        id: popover

        Popover {
            Column {
                anchors {
                    left: parent.left
                    top: parent.top
                    right: parent.right
                }
                ListItem.Standard { text: "Do something" }
                ListItem.Standard { text: "Do something else" }
                ListItem.Subtitled { text: "Title"; subText: "Subtitle"; showDivider: false}
            }
        }
    }

    Button {
        id: referenceButton
        visible: false
    }

    Grid {
        id: grid
        anchors.fill: parent
        columns: 6
        rows: 8
        spacing: units.gu(6)
        anchors.leftMargin: -referenceButton.width/2.0
        anchors.topMargin: -referenceButton.height/2.0

        Repeater {
            model: grid.columns * grid.rows
            delegate: Button {
                id: button
                text: "open"
                onClicked: PopupUtils.open(popover, button)
            }
        }
    }
}

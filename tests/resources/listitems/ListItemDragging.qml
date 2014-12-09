/*
 * Copyright 2014 Canonical Ltd.
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
import Ubuntu.Components 1.2

MainView {
    id: main
    width: units.gu(50)
    height: units.gu(105)
    useDeprecatedToolbar: false

    Page {
        title: "Dragging test"
        ListView {
            anchors.fill: parent
            ListItem.selectable: ListItem.draggable

            model: ListModel {
                Component.onCompleted: {
                    for (var i = 0; i < 25; i++) {
                        append({label: "List item #"+i})
                    }
                }
            }

            delegate: ListItem {
                Label {
                    text: label
                }
                Button {
                    text: "Press"
                    anchors.centerIn: parent
                }

                onPressAndHold: {
                    print("entering draggable mode")
                    ListView.view.ListItem.draggable = true;
                }
                onDraggingChanged: print("dragging", dragging)
            }
        }
    }
}

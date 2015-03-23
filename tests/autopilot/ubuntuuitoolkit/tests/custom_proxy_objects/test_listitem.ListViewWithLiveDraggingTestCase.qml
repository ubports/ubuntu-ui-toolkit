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

import QtQuick 2.0
import Ubuntu.Components 1.2

MainView {
    width: units.gu(48)
    height: units.gu(60)
    objectName: "mainView"

    Page {
        id: testPage
        objectName: "test_page"
        title: listView.ViewItems.dragMode ? "In drag mode" : "Test app"
        ListView {
            id: listView
            objectName: "test_view"
            ViewItems.objectName: "test_attached"
            ViewItems.onDragUpdated: {
                if (event.status == ListItemDrag.Moving) {
                    model.move(event.from, event.to, 1)
                }
            }

            anchors.fill: parent
            model: ListModel {
                Component.onCompleted: {
                    for (var i = 0; i < 25; i++) {
                        append({data: i});
                    }
                }
            }
            moveDisplaced: Transition {
                UbuntuNumberAnimation {
                    properties: "y";
                }
            }
            delegate: ListItem {
                objectName: "listitem" + index
                ListView.objectName: "bumm"
                onPressAndHold: ListView.view.ViewItems.dragMode = !ListView.view.ViewItems.dragMode
                Label {
                    text: "List item #" + modelData
                }
            }
        }
    }
}

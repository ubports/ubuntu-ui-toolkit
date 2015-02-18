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

import QtQuick 2.3
import Ubuntu.Components 1.2

MainView {
    id: main
    width: units.gu(40)
    height: units.gu(60)
    useDeprecatedToolbar: false

    property bool restrictDragging: true

    Page {
        title: "Dragging test"
        ListView {
            anchors.fill: parent
            ViewItems.selectMode: ViewItems.dragMode
            ViewItems.onDraggingStarted: {
                if (!restrictDragging) {
                    return;
                }

                if (event.from < 3) {
                    // the first 3 items are not draggable
                    event.accept = false;
                } else if (event.from >=3 && event.from <= 10) {
                    // dragging is limited between index 3 and 10
                    event.minimumIndex = 3;
                    event.maximumIndex = 10;
                } else {
                    // prevent dragging above index 10
                    event.minimumIndex = 11;
                }
            }

            ViewItems.onDraggingUpdated: {
                if (restrictDragging) {
                    // deal only with indexes >= 3
                    if (event.to >= 3 && event.to <= 10 && event.from >= 3 && event.from <= 10) {
                        // live update
                        model.move(event.from, event.to, 1);
                    } else if (event.direction == ListItemDrag.Steady) {
                        model.move(event.from, event.to, 1);
                    } else {
                        event.accept = false;
                    }
                } else {
                    // no restrictions, perform live update
                    model.move(event.from, event.to, 1);
                }
            }

            model: ListModel {
                Component.onCompleted: {
                    for (var i = 0; i < 25; i++) {
                        append({label: "List item #"+i})
                    }
                }
            }

            moveDisplaced: Transition {
                UbuntuNumberAnimation {
                    properties: "x,y"
                }
            }

            delegate: ListItem {
                id: item
                objectName: "ListItem-" + index
                leadingActions: ListItemActions {
                    actions: Action {
                        iconName: "delete"
                    }
                }

                Rectangle {
                    anchors.fill: parent
                    color: item.dragging ? UbuntuColors.blue : "#69aa69"
                }
                Label {
                    text: label + " from index #" + index
                }

                onPressAndHold: {
                    print("entering/leaving draggable mode")
                    ListView.view.ViewItems.dragMode = !ListView.view.ViewItems.dragMode;
                }
            }
        }
    }
}

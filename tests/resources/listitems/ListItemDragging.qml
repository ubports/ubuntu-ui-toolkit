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
import Ubuntu.Components 1.2
import QtQuick.Layouts 1.1
import Ubuntu.Components.ListItems 1.0
import QtQml.Models 2.1

MainView {
    id: main
    width: units.gu(40)
    height: units.gu(71)

    property bool restrictDragging: true

    LayoutMirroring.childrenInherit: true

    Action {
        id: deleteAction
        iconName: "delete"
    }
    property list<Action> contextualActions: [
        Action {
            iconName: "edit"
        },
        Action {
            iconName: "share"
        },
        Action {
            iconName: "stock_website"
        }
    ]
    Tabs {
        Tab {
            title: "Dragging with ListModel"
            page: Page {
                ListView {
                    onEnabledChanged: print("enabled", enabled)
                    anchors.fill: parent
                    ViewItems.selectMode: ViewItems.dragMode
                    ViewItems.onSelectedIndicesChanged: print(ViewItems.selectedIndices)

                    function handleDragStarted(event) {
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
                    function handleDragUpdates(event) {
                        if (restrictDragging) {
                            // deal only with indexes >= 3
                            if (event.to >= 3 && event.to <= 10 && event.from >= 3 && event.from <= 10) {
                                // live update
                                model.move(event.from, event.to, 1);
                            } else if (event.status == ListItemDrag.Dropped) {
                                model.move(event.from, event.to, 1);
                            } else {
                                event.accept = false;
                            }
                        } else {
                            // no restrictions, perform live update
                            model.move(event.from, event.to, 1);
                        }
                    }

                    ViewItems.onDragUpdated: {
                        if (event.status == ListItemDrag.Started) {
                            handleDragStarted(event);
                        } else {
                            handleDragUpdates(event);
                        }
                    }

                    model: ListModel {
                        Component.onCompleted: {
                            for (var i = 0; i < 3; i++) {
                                append({label: "List item #"+i, sectionData: "Locked"});
                            }
                            for (i = 3; i < 11; i++) {
                                append({label: "List item #"+i, sectionData: "Limited, live move"});
                            }
                            for (i = 11; i < 25; i++) {
                                append({label: "List item #"+i, sectionData: "Unlimited, drag'n'drop"});
                            }
                        }
                    }

                    section {
                        property: "sectionData"
                        criteria: ViewSection.FullString
                        delegate: Header {
                            text: section
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
                            actions: deleteAction
                        }
                        trailingActions: ListItemActions {
                            actions: contextualActions
                        }

                        RowLayout {
                            anchors {
                                fill: parent
                                margins: units.gu(0.5)
                                leftMargin: anchors.rightMargin
                                rightMargin: units.gu(2)
                            }
                            Captions {
                                id: captions
                                title.text: label
                                subtitle.text: "from index #" + index
                            }
                            Captions {
                                captionStyle: Ubuntu.SummaryCaptionStyle
                                title.text: "LTR"
                                subtitle.text: "RTL"
                            }
                        }

                        onPressAndHold: {
                            print("entering/leaving draggable mode")
                            ListView.view.ViewItems.dragMode = !ListView.view.ViewItems.dragMode;
                        }
                        onClicked: main.LayoutMirroring.enabled = !main.LayoutMirroring.enabled
                    }
                }
            }
        }
        Tab {
            title: "Using DelegateModel"
            page: Page {
                ListView {
                    anchors.fill: parent
                    model: DelegateModel {
                        model: ["apple", "pear", "plum", "peach", "nuts", "dates"]
                        delegate: ListItem { Label { text: modelData } onPressAndHold: dragMode = !dragMode; }
                    }
                    ViewItems.onDragUpdated: {
                        if (event.status == ListItemDrag.Moving) {
                            event.accept = false
                        } else if (event.status == ListItemDrag.Dropped) {
                            var fromData = model.model[event.from];
                            var list = model.model;
                            list.splice(event.from, 1);
                            list.splice(event.to, 0, fromData);
                            model.model = list;
                        }
                    }
                }
            }
        }
    }
}

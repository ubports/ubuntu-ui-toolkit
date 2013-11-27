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


MainView {

    id: root
    width: units.gu(40)
    height: units.gu(71)

//    property var colorModel: ["red", "green", "blue", "purple"]
    ListModel {
        id: colorModel
        ListElement {
            color: "red"
        }
        ListElement {
            color: "green"
        }
        ListElement {
            color: "blue"
        }
        ListElement {
            color: "purple"
        }
    }

    ListModel {
        id: inputModel
        Component.onCompleted: {
            append({ "name": "tab 1" });
            insert(0, { "name": "tab 0" });
            append({ "name": "tab 3" });
            insert(2, { "name": "tab 2" });
        }
    }
    property var listModel: ["tab #0", "tab #1", "tab #2", "tab #3"];

    Tabs {
        id: tabsWithRepeater
        Repeater {
            objectName: "first_repeater"
            id: tabsRepeater
            model: inputModel
            Tab {
                title: name
                page: Page {
                    Label {
                        anchors.centerIn: parent
                        text: title
                        fontSize: "large"
                    }
                    Item {
                        Row {
                            id: colorRow
                            Repeater {
                                model: colorModel
                                Rectangle {
                                    objectName: modelData
                                    width: 40; height: 40
                                    color: modelData
                                }
                            }
                            Repeater {
                                model: colorModel
                                Rectangle {
                                    objectName: modelData
                                    width: 40; height: 40
                                    color: modelData
                                }
                            }
                        }
                    }

                    tools: ToolbarItems {
                        ToolbarButton {
                            text: "shufle"
                            onTriggered: {
                                inputModel.move(1, 2, 1);
                                inputModel.move(3, 0, 1);
                                inputModel.move(1, 3, 1);
                            }
                        }
                        ToolbarButton {
                            text: "reset"
                            onTriggered: tabsRepeater.model = null;
                        }
                        ToolbarButton {
                            text: "recolorize"
                            onTriggered: {
//                                var x = list[2];
//                                list[2] = list[1];
//                                list[1] = x;
//                                print("new list=" + list)
                                colorModel.move(1, 2, 1);
                                for (var i = 0; i < colorRow.children.length; i++) {
                                    if (QuickUtils.className(colorRow.children[i]) === "QQuickRectangle")
                                    {
                                        print("row @" + i + " is" + colorRow.children[i].objectName)
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        Repeater {
            objectName: "second_repeater"
            id: secondRepeater
            model: inputModel//listModel
            Tab {
                title: "second" + modelData
                page: Page { Label { text: title } }
            }
        }
    }
}

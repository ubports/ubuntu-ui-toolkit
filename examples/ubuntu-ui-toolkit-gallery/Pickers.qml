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
import Ubuntu.Components 0.1
import Ubuntu.Components.Pickers 0.1

Template {
    TemplateSection {
        className: "Picker"

        TemplateRow {
            title: i18n.tr("Linear")
            Picker {
                circular: false
                model: ["Line1", "Line2", "Line3", "Line4", "Line5", "Line6", "Line7", "Line8", "Line9", "Line10"]
                delegate: PickerDelegate{
                    Label {
                        text: modelData
                        anchors.verticalCenter: parent.verticalCenter
                    }
                }

                onSelectedIndexChanged: print("index=" + selectedIndex)
            }
        }

        TemplateRow {
            title: i18n.tr("Circular")
            Picker {
                delegate: PickerDelegate {
                    Label {
                        text: modelData
                        anchors.verticalCenter: parent.verticalCenter
                    }
                }

                onSelectedIndexChanged: print("index=" + selectedIndex)

                Component.onCompleted: {
                    var stack = []
                    for (var i = 0; i < 100; i++)
                        stack.push("Line " + i)
                    model = stack;
                    selectedIndex = 3;
                }
            }
        }

        TemplateRow {
            title: i18n.tr("Infinite")
            Picker {
                id: picker
                model: ListModel {}
                circular: false
                delegate: PickerDelegate{
                    Label {
                        text: modelData
                        anchors.verticalCenter: parent.verticalCenter
                    }
                    Component.onCompleted: {
                        if (index === (picker.model.count - 1))
                            picker.expandModel()
                    }
                }

                function expandModel() {
                    for (var i = 0; i < 50; i++) {
                        picker.model.append({"blob": "Line " + picker.model.count});
                    }
                }
                Component.onCompleted: {
                    for (var i = 0; i < 100; i++)
                        model.append({"blob" : "Line " + i})
                }
            }
        }
    }
}

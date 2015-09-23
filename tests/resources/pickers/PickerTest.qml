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
import Ubuntu.Components 1.3
import Ubuntu.Components.Pickers 1.3

MainView {
    width: units.gu(40)
    height: units.gu(71)
    Column{
        Row {
            ListModel {
                id: pickerModel
                Component.onCompleted: reset(100);

                function reset(num) {
                    clear();
                    for (var i = 0; i < num; i++) {
                        append({"label": "Item #"+i});
                    }
                }
            }


            Picker {
                objectName: "Circular"
                id: circularPicker
                height: units.gu(40)
                model: pickerModel
                delegate: PickerDelegate {
                    property string text: modelData ? modelData : ""
                    Label {
                        anchors.fill: parent
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        text: modelData ? modelData : ""
                    }
                }
                onSelectedIndexChanged: print("circular index="+selectedIndex)
                onMovingChanged: print("MOVING?", moving)
            }

            Picker {
                objectName: "Linear"
                id: linearPicker
                height: units.gu(40)
                model: pickerModel
                circular: false
                delegate: PickerDelegate {
                    property string text: modelData ? modelData : ""
                    Label {
                        anchors.fill: parent
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        text: modelData ? modelData : ""
                    }
                }
                onSelectedIndexChanged: print("linear index="+selectedIndex)
                onMovingChanged: print("MOVING?", moving)
            }

            Picker {
                height: units.gu(40)
                model: pickerModel
                live: true
                delegate: PickerDelegate {
                    Label {
                        anchors.fill: parent
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        text: modelData
                    }
                }
                onSelectedIndexChanged: print("circular index="+selectedIndex)
                onMovingChanged: print("MOVING?", moving)
            }

            Picker {
                height: units.gu(40)
                model: pickerModel
                circular: false
                live: true
                itemHeight: units.gu(10)
                delegate: PickerDelegate {
                    Label {
                        anchors.fill: parent
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        text: modelData
                    }
                }
                onSelectedIndexChanged: print("linear index="+selectedIndex)
                onMovingChanged: print("MOVING?", moving)
            }
        }

        Flow {
            spacing: units.gu(1)
            width: parent.width

            Button {
                text: "move & crop from 50"
                onClicked: {
                    linearPicker.selectedIndex = 50;
                    circularPicker.selectedIndex = 50;
                    pickerModel.remove(50, 50);
                    print("circular: selectedIndex="+circularPicker.selectedIndex)
                    print("linear: selectedIndex="+linearPicker.selectedIndex)
                }
            }
            Button {
                text: "crop from 50"
                onClicked: {
                    pickerModel.remove(50, 50);
                    print("circular: selectedIndex="+circularPicker.selectedIndex)
                    print("linear: selectedIndex="+linearPicker.selectedIndex)
                }
            }
            Button {
                text: "reset"
                onClicked: {
                    pickerModel.reset(100);
                    print("circular: selectedIndex="+circularPicker.selectedIndex)
                    print("linear: selectedIndex="+linearPicker.selectedIndex)
                }
            }
            Button {
                text: "move to 20"
                onClicked: {
                    circularPicker.selectedIndex = 20;
                    linearPicker.selectedIndex = 20;
                }
            }

            Button {
                text: "clear"
                onClicked: {
                    pickerModel.clear();
                    print("circular: selectedIndex="+circularPicker.selectedIndex)
                    print("linear: selectedIndex="+linearPicker.selectedIndex)
                }
            }
            Button {
                text: "append"
                onClicked: {
                    pickerModel.append({"label": "Item #"+pickerModel.count});
                }
            }
        }
    }
}

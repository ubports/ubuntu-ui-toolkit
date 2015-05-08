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
import Ubuntu.Components 1.3
import Ubuntu.Components.Pickers 1.3

Template {
    objectName: "pickersTemplate"
    id: root
    TemplateSection {
        className: "Picker"
        documentation: "qml-ubuntu-components-pickers-picker.html"

        TemplateRow {
            title: i18n.tr("Linear")
            Picker {
                circular: false
                model: ["Line1", "Line2", "Line3", "Line4", "Line5", "Line6", "Line7", "Line8", "Line9", "Line10"]

                delegate: PickerDelegate {
                    Label {
                        text: modelData
                        anchors.fill: parent
                        verticalAlignment: Text.AlignVCenter
                        horizontalAlignment: Text.AlignHCenter
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
                        anchors.fill: parent
                        verticalAlignment: Text.AlignVCenter
                        horizontalAlignment: Text.AlignHCenter
                    }
                }

                onSelectedIndexChanged: print("index=" + selectedIndex)

                Component.onCompleted: {
                    var stack = [];
                    for (var i = 0; i < 100; i++) {
                        stack.push("Line " + i);
                    }
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

                delegate: PickerDelegate {
                    Label {
                        text: modelData
                        anchors.fill: parent
                        verticalAlignment: Text.AlignVCenter
                        horizontalAlignment: Text.AlignHCenter
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

    TemplateSection {
        className: "Dialer"
        documentation: "qml-ubuntu-components-pickers-dialer.html"

        TemplateRow {
            title: i18n.tr("Clock")
            Dialer {
                size: units.gu(20)
                handSpace: units.gu(4)
                minimumValue: 0
                maximumValue: 60

                DialerHand {
                    id: hour
                    hand.toCenterItem: true
                    value: new Date().getHours() * 5
                }
                DialerHand {
                    id: minute
                    value: new Date().getMinutes()
                }

                DialerHand {
                    id: second
                    value: new Date().getSeconds()
                }

                centerContent: [
                    Label {
                        id: hourLabel
                        anchors.centerIn: parent
                    }
                ]

                onHandUpdated: {
                    hourLabel.text = Math.round(hour.value / 5) + ":"
                            + Math.round(minute.value) + ":"
                            + Math.round(second.value);
                }
            }
        }
        TemplateRow {
            title: i18n.tr("Overlay")
            Dialer {
                size: units.gu(20)
                handSpace: units.gu(4)

                DialerHand {
                    id: selector
                    hand.visible: false
                    value: 311
                    Rectangle {
                        anchors.centerIn: parent
                        width: height
                        height: units.gu(3)
                        radius: width / 2
                        color: theme.palette.normal.background
                        antialiasing: true
                        Label {
                            text: Math.round(selector.value)
                            anchors.centerIn: parent
                        }
                    }
                }

                centerContent: [
                    Label {
                        id: handText
                        anchors.centerIn: parent
                    }
                ]

                onHandUpdated: handText.text = Math.round(hand.value);
            }
        }
    }
    TemplateSection {
        className: "DatePicker"
        documentation: "qml-ubuntu-components-pickers-datepicker.html"
        TemplateRow {
            title: "Date"
            DatePicker {
                // make sure we have the whole component in screen
                width: Math.min(root.width - units.gu(16), units.gu(40))
                onDateChanged: print("picked date="+Qt.formatDate(date, "yyyy/MMMM/dd"))
            }
        }
        TemplateRow {
            title: "Month"
            DatePicker {
                mode: "Years|Months"
                // make sure we have the whole component in screen
                width: Math.min(root.width - units.gu(16), units.gu(40))
                onDateChanged: print("picked month="+Qt.formatDate(date, "yyyy/MMMM"))
            }
        }
        TemplateRow {
            title: "Time"
            DatePicker {
                mode: "Hours|Minutes|Seconds"
                date: new Date()
                // make sure we have the whole component in screen
                width: Math.min(root.width - units.gu(16), units.gu(40))
                onDateChanged: print("picked time="+Qt.formatTime(date, "hh:mm:ss"))
            }
        }
    }

    TemplateSection {
        title: "PickerPanel"
        TextField {
            id: textField
            readOnly: true
            property date date: new Date()
            text: date.toISOString()
            Mouse.onClicked: PickerPanel.openDatePicker(textField, "date");
        }
    }
}

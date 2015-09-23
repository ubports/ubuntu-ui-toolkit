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
import Ubuntu.Components 1.1
import Ubuntu.Components.Pickers 1.0

MainView {
    width: units.gu(40)
    height: units.gu(71)

    Page {
        title: "DatePicker test"
        Column {
            anchors.fill: parent
            DatePicker {
                id: picker
                locale: Qt.locale("en_UK")
                date: new Date()

                onDateChanged: print("CHANGED DATE=" + Qt.formatDateTime(date, "yyyy/MM/dd, hh:mm:ss"))
                onMovingChanged: print("MOVING?", moving)
            }
            Slider {
                value: 0.0//units.gu(36)
                live: true
                height: units.gu(2)
                minimumValue: 0.0
                maximumValue: parent.width
                onValueChanged: picker.width = value
            }

            Flow {
                width: parent.width
                spacing: units.gu(1)
                Button {
                    text: "HU"
                    width: units.gu(5)
                    height: units.gu(2)
                    onClicked: picker.locale = Qt.locale("hu_HU")
                }
                Button {
                    text: "DE"
                    width: units.gu(5)
                    height: units.gu(2)
                    onClicked: picker.locale = Qt.locale("de_DE")
                }
                Button {
                    text: "EN(US)"
                    width: units.gu(9)
                    height: units.gu(2)
                    onClicked: picker.locale = Qt.locale("en_US")
                }
                Button {
                    text: "FI"
                    width: units.gu(5)
                    height: units.gu(2)
                    onClicked: {
                        picker.locale = Qt.locale("fi_FI")
                        picker.width = units.gu(37);
                    }
                }

                Button {
                    text: "infinite"
                    height: units.gu(2)
                    onClicked: picker.maximum = picker.maximum.getInvalidDate()
                }
                Button {
                    text: "Jan-Mar"
                    height: units.gu(2)
                    onClicked: {
                        var date = new Date();
                        date.setMonth(0);
                        picker.minimum = date;
                        date.setMonth(2);
                        picker.maximum = date;
                        picker.date = new Date(date);
                    }
                }
                Button {
                    text: "Change minimum"
                    height: units.gu(2)
                    onClicked: {
                        picker.minimum = new Date(2012, 11, 1);
                    }
                }
                Button {
                    text: "minimum & date"
                    height: units.gu(2)
                    onClicked: {
                        picker.minimum = new Date(2013, 11, 1);
                        picker.date = new Date(2014, 11, 5);
                    }
                }
                Button {
                    text: "today"
                    height: units.gu(2)
                    onClicked: picker.date = new Date()
                }

                Repeater {
                    model: ["Years|Months|Days", "Years|Months", "Months|Days", "Years", "Months", "Days",
                        "Hours|Minutes|Seconds", "Hours|Minutes", "Minutes|Seconds", "Hours", "Minutes", "Seconds",
                        "Years|Months|Days|Hours|Minutes"]
                    Button {
                        text: "model: " + modelData
                        height: units.gu(2)
                        onClicked: picker.mode = modelData
                    }
                }
                Button {
                    text: "disable hours"
                    height: units.gu(2)
                    onClicked: {
                        picker.mode = "Hours|Minutes|Seconds";
                        var min = new Date();
                        min.setHours(23); min.setMinutes(0); min.setSeconds(0);
                        var max = new Date();
                        max.setHours(23); max.setMinutes(59); max.setSeconds(59);
                        picker.minimum = min;
                        picker.maximum = max;
                    }
                }
                Button {
                    text: "disable hours and minutes"
                    height: units.gu(2)
                    onClicked: {
                        picker.mode = "Hours|Minutes|Seconds";
                        var min = new Date();
                        min.setHours(23); min.setMinutes(10); min.setSeconds(0);
                        var max = new Date();
                        max.setHours(23); max.setMinutes(10); max.setSeconds(59);
                        picker.minimum = min;
                        picker.maximum = max;
                    }
                }
                Button {
                    text: "second range"
                    height: units.gu(2)
                    onClicked: {
                        picker.mode = "Hours|Minutes|Seconds";
                        var min = new Date();
                        min.setHours(23); min.setMinutes(10); min.setSeconds(20);
                        var max = new Date();
                        max.setHours(23); max.setMinutes(10); max.setSeconds(59);
                        picker.minimum = min;
                        picker.maximum = max;
                    }
                }
            }
        }
    }
}

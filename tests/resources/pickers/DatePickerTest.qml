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
import Ubuntu.Components.Pickers 0.1

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

                onDateChanged: print("CHANGED DATE=" + Qt.formatDate(date, "yyyy/MM/dd"))
            }
            Slider {
                value: 0.0//units.gu(36)
                live: true
                minimumValue: 0.0
                maximumValue: parent.width
                onValueChanged: picker.width = value
            }

            Flow {
                width: parent.width
                spacing: units.gu(1)
                Button {
                    text: "HU"
                    onClicked: picker.locale = Qt.locale("hu_HU")
                }
                Button {
                    text: "DE"
                    onClicked: picker.locale = Qt.locale("de_DE")
                }
                Button {
                    text: "EN(US)"
                    onClicked: picker.locale = Qt.locale("en_US")
                }

                Button {
                    text: "infinite"
                    onClicked: picker.maximum = picker.maximum.getInvalidDate()
                }
                Button {
                    text: "Jan-Mar"
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
                    onClicked: {
                        picker.minimum = new Date("2012/12/1");
                    }
                }
                Button {
                    text: "minimum & date"
                    onClicked: {
                        picker.minimum = new Date("2013/12/1");
                        picker.date = new Date("2014/12/5");
                    }
                }
                Button {
                    text: "today"
                    onClicked: picker.date = new Date()
                }

                Repeater {
                    model: ["Year|Month|Day", "Year|Month", "Month|Day", "Year", "Month", "Day", "Hours"]
                    Button {
                        text: "model: " + modelData
                        onClicked: picker.mode = modelData
                    }
                }
            }
        }
    }
}

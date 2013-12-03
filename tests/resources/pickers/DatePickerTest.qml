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
            DatePicker {
                id: picker
                locale: Qt.locale("en_UK")
//                minimum: new Date("2013/12/1")
//                maximum: new Date("2013/12/31")

                onDateChanged: print("CHANGED DATE=" + Qt.formatDate(date, "yyyy/MM/dd"))
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
                text: "minimum & date"
                onClicked: {
                    picker.minimum = new Date("2013/12/1");
//                    picker.date = new Date("2013/12/31");
                }
            }
        }
    }
}

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
import QtTest 1.0
import Ubuntu.Test 0.1
import Ubuntu.Components 0.1
import Ubuntu.Components.Pickers 0.1

Item {
    id: testSuite
    width: units.gu(40)
    height: units.gu(71)

    DatePicker {
        id: picker
    }

    SignalSpy {
        id: modeChange
        target: picker
        signalName: "modeChanged"
    }

    SignalSpy {
        id: dateChange
        target: picker
        signalName: "dateChanged"
    }

    SignalSpy {
        id: yearChange
        target: picker
        signalName: "yearChanged"
    }

    SignalSpy {
        id: monthChange
        target: picker
        signalName: "monthChanged"
    }

    SignalSpy {
        id: dayChange
        target: picker
        signalName: "dayChanged"
    }

    SignalSpy {
        id: weekChange
        target: picker
        signalName: "weekChanged"
    }

    UbuntuTestCase {
        name: "DatePickerAPI"
        when: windowShown

        function test_0_mode() {
            compare(picker.mode, "Date", "default mode");
        }
        function test_0_date() {
            var date = (new Date()).midnight();
            compare(picker.date, date, "default mode");
            compare(picker.year, date.getFullYear(), "default year");
            compare(picker.month, date.getMonth(), "default month");
            compare(picker.day, date.getDate(), "default day");
            compare(picker.week, date.getWeek(), "default week");
        }
        function test_0_live() {
            compare(picker.live, false, "default live mode");
        }
        function test_0_minimum_maximum() {
            var endDate = Date.prototype.midnight.call(new Date());
            endDate.setFullYear(endDate.getFullYear() + 50);
            compare(picker.minimum, picker.date, "default minimum is same as date property");
            compare(picker.maximum, endDate, "default maximum is a 50 years window");
        }
        function test_0_locale() {
            compare(picker.locale, Qt.locale(), "default locale is the same as system locale");
        }

        function test_1_localeHu() {
            var locale = Qt.locale("hu_HU");
            picker.minimum = new Date("2012/12/1");
            picker.date = new Date("2012/12/1");
            picker.locale = Qt.locale("hu_HU");
            waitForRendering(picker);
            wait(4000);
            var pickerItem = findChild(picker, "DatePicker_MonthPicker");
            var label = findChild(pickerItem.itemList.currentItem, "DatePicker_PickerLabel");
            compare(label.text, locale.monthName(picker.date.getMonth(), Locale.LongFormat), "locale for month wrong");
//            pickerItem = findChild(picker, "DatePicker_DayPicker");
//            label = findChild(pickerItem.itemList.currentItem, "DatePicker_PickerLabel");
//            compare(label.text, locale.dayName(picker.date.getDate(), Locale.LongFormat), "locale for day name wrong");
        }
    }
}

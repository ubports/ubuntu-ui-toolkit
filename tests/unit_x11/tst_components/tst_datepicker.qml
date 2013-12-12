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
        width: parent.width
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

        function getPickerLabel(picker, name) {
            var pickerItem = findChild(picker, name);
            var pickerCurrent = findChild(pickerItem, "Picker_ViewLoader");
            return findChild(pickerCurrent.item.currentItem, "DatePicker_PickerLabel");
        }
        function getPickerModel(picker, name) {
            var pickerItem = findChild(picker, name);
            return pickerItem ? pickerItem.model : undefined;
        }

        function test_0_mode() {
            compare(picker.mode, "Y|M|D", "default mode");
        }
        function test_0_date() {
            var date = (new Date()).midnight();
            compare(picker.date, date, "default mode");
            compare(picker.year, date.getFullYear(), "default year");
            compare(picker.month, date.getMonth(), "default month");
            compare(picker.day, date.getDate(), "default day");
            compare(picker.week, date.getWeek(), "default week");
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

        function test_1_changeModeYM() {
            var oldMode = picker.mode;

            var newMode = "Y|M";
            var pickerCount = 2 + 1; // +1 is the Repeater
            picker.mode = newMode;
            wait(500);
            var positioner = findChild(picker, "DatePicker_Positioner");
            compare(positioner.children.length, pickerCount, "invalid amount of pickers");

            picker.mode = "Year|Month|Day";
            pickerCount = 3 + 1; // +1 is the Repeater
            compare(positioner.children.length, pickerCount, "invalid amount of pickers");
        }

        function test_1_changeModeMD() {
            var oldMode = picker.mode;

            var newMode = "D|M";
            var pickerCount = 2 + 1; // +1 is the Repeater
            picker.mode = newMode;
            wait(500);
            var positioner = findChild(picker, "DatePicker_Positioner");
            compare(positioner.children.length, pickerCount, "invalid amount of pickers");

            picker.mode = "Year|Month|Day";
            pickerCount = 3 + 1; // +1 is the Repeater
            compare(positioner.children.length, pickerCount, "invalid amount of pickers");
        }

        function test_1_changeModeYS() {
            var oldMode = picker.mode;

            var newMode = "Y|S";
            var pickerCount = 2 + 1; // +1 is the Repeater
            picker.mode = newMode;
            wait(500);
            var positioner = findChild(picker, "DatePicker_Positioner");
            expectFailContinue("", "Invalid mode");
            compare(positioner.children.length, pickerCount, "invalid amount of pickers");

            picker.mode = "Year|Month|Day";
            pickerCount = 3 + 1; // +1 is the Repeater
            compare(positioner.children.length, pickerCount, "invalid amount of pickers");
        }

        function test_1_changeModeY() {
            var oldMode = picker.mode;

            var newMode = "Y";
            var pickerCount = 1 + 1; // +1 is the Repeater
            picker.mode = newMode;
            wait(500);
            var positioner = findChild(picker, "DatePicker_Positioner");
            compare(positioner.children.length, pickerCount, "invalid amount of pickers");

            picker.mode = "Year|Month|Day";
            pickerCount = 3 + 1; // +1 is the Repeater
            compare(positioner.children.length, pickerCount, "invalid amount of pickers");
        }

        function test_1_changeModeM() {
            var oldMode = picker.mode;

            var newMode = "M";
            var pickerCount = 1 + 1; // +1 is the Repeater
            picker.mode = newMode;
            wait(500);
            var positioner = findChild(picker, "DatePicker_Positioner");
            compare(positioner.children.length, pickerCount, "invalid amount of pickers");

            picker.mode = "Year|Month|Day";
            pickerCount = 3 + 1; // +1 is the Repeater
            compare(positioner.children.length, pickerCount, "invalid amount of pickers");
        }

        function test_1_changeModeD() {
            var oldMode = picker.mode;

            var newMode = "D";
            var pickerCount = 1 + 1; // +1 is the Repeater
            picker.mode = newMode;
            wait(500);
            var positioner = findChild(picker, "DatePicker_Positioner");
            compare(positioner.children.length, pickerCount, "invalid amount of pickers");

            picker.mode = "Year|Month|Day";
            pickerCount = 3 + 1; // +1 is the Repeater
            compare(positioner.children.length, pickerCount, "invalid amount of pickers");
        }

        function test_1_changeLocale() {
            var prevLocale = picker.locale;
            var locale = Qt.locale("hu_HU");
            picker.minimum = new Date("2012/12/1");
            picker.date = new Date("2012/12/1");
            wait(500)
            picker.locale = Qt.locale("hu_HU");
            wait(500)
            var label = getPickerLabel(picker, "DatePicker_MonthPicker");
            compare(label.text, locale.monthName(picker.date.getMonth(), Locale.LongFormat), "locale for month wrong");

            label = getPickerLabel(picker, "DatePicker_DayPicker");
            var dayModel = getPickerModel(picker, "DatePicker_DayPicker");
            compare(label.text, dayModel.text(picker.date.getDate() - 1, testSuite.width), "locale for day name wrong");
            picker.locale = prevLocale;
        }

        function test_1_changeMinimumBeforeDate() {
            var date = new Date(picker.date);
            var originalDate = new Date(date);
            date.setFullYear(date.getFullYear() - 1);
            date.setDate(1);
            picker.minimum = date;
            wait(500);
            var year = getPickerLabel(picker, "DatePicker_YearPicker");
            compare(year.text, originalDate.getFullYear().toString(), "year differs");
            var month = getPickerLabel(picker, "DatePicker_MonthPicker");
            compare(month.text, picker.locale.monthName(originalDate.getMonth(), Locale.LongFormat), "month differs");
            var day = getPickerLabel(picker, "DatePicker_DayPicker");
            var dayModel = getPickerModel(picker, "DatePicker_DayPicker");
            compare(day.text, dayModel.text(originalDate.getDate() - 1), "day differs");
        }

        function test_1_changeMaximumAfterDate() {
            var date = new Date(picker.date);
            var originalDate = new Date(date);
            date.setFullYear(date.getFullYear() + 1);
            date.setDate(1);
            picker.maximum = date;
            wait(500);
            var year = getPickerLabel(picker, "DatePicker_YearPicker");
            compare(year.text, originalDate.getFullYear().toString(), "year differs");
            var month = getPickerLabel(picker, "DatePicker_MonthPicker");
            compare(month.text, picker.locale.monthName(originalDate.getMonth(), Locale.LongFormat), "month differs");
            var day = getPickerLabel(picker, "DatePicker_DayPicker");
            var dayModel = getPickerModel(picker, "DatePicker_DayPicker");
            compare(day.text, dayModel.text(originalDate.getDate() - 1), "day differs");
        }

        // make infinite
        function test_1_changeMinimumInvalid() {
            picker.minimum = Date.prototype.getInvalidDate.call();
            compare(picker.minimum, picker.date, "invalid minimum hasn't been adjusted to date");
        }

        // make infinite
        function test_1_changeMaximumInvalid() {
            picker.maximum = Date.prototype.getInvalidDate.call();

            // check if the year picker model is autoExtending
            var yearModel = getPickerModel(picker, "DatePicker_YearPicker");
            compare(yearModel.autoExtend, true, "the year picker is not auto-extending one");
        }

        function test_1_changeDate() {
            var date = picker.date;
            date.setFullYear(date.getFullYear() + 2);
            date.setMonth(5);
            date.setDate(21);
            picker.date = date;
            picker.mode = "Y|M|D";
            wait(500);

            var yearLabel = getPickerLabel(picker, "DatePicker_YearPicker");
            var monthLabel = getPickerLabel(picker, "DatePicker_MonthPicker");
            var monthModel = getPickerModel(picker, "DatePicker_MonthPicker");
            var dayLabel = getPickerLabel(picker, "DatePicker_DayPicker");
            var dayModel = getPickerModel(picker, "DatePicker_DayPicker");
            compare(yearLabel.text, date.getFullYear().toString(), "different year value");
            compare(monthLabel.text, monthModel.text(date.getMonth()), "different month value");
            compare(dayLabel.text, dayModel.text(date.getDate() - 1), "different day value");
        }

        function test_1_changeDateToNextMonth() {
            picker.date = new Date("2013/10/31");
            picker.locale = Qt.locale("hu_HU")
            wait(500);

            // click on the month picker to set the next month
            var monthPicker = findChild(picker, "DatePicker_MonthPicker");
            var monthCurrent = findChild(monthPicker, "Picker_ViewLoader");
            var my = monthPicker.y + (monthPicker.height / 2) + monthCurrent.item.currentItem.height;
            var mx = monthPicker.x + monthPicker.width / 2;
            mouseClick(testSuite, mx, my);
            wait(500);

            var yearLabel = getPickerLabel(picker, "DatePicker_YearPicker");
            var monthLabel = getPickerLabel(picker, "DatePicker_MonthPicker");
            var monthModel = getPickerModel(picker, "DatePicker_MonthPicker");
            var dayLabel = getPickerLabel(picker, "DatePicker_DayPicker");
            var dayModel = getPickerModel(picker, "DatePicker_DayPicker");
            compare(yearLabel.text, "2013", "different year value");
            // November
            compare(monthLabel.text, monthModel.text(10), "different month value");
            // the 30th
            compare(dayLabel.text, dayModel.text(29), "different day value");

            // set it back
            my = monthPicker.y + (monthPicker.height / 2) - monthCurrent.item.currentItem.height;
            mouseClick(testSuite, mx, my);
            wait(500);

            compare(yearLabel.text, "2013", "different year value");
            // October
            monthLabel = getPickerLabel(picker, "DatePicker_MonthPicker");
            compare(monthLabel.text, monthModel.text(9), "different month value");
            // the 30th
            dayLabel = getPickerLabel(picker, "DatePicker_DayPicker");
            compare(dayLabel.text, dayModel.text(29), "different day value");
        }

        function test_2_disabledYear() {
            var date = new Date("2013/5/1");
            var minDate = new Date("2013/1/1");
            var maxDate = new Date("2013/12/31");
            picker.minimum = minDate;
            picker.maximum = maxDate;
            wait(500);
            picker.date = date;

            var yearPicker = findChild(picker, "DatePicker_YearPicker");
            compare(yearPicker.enabled, false, "year picker should be disabled");
        }

        function test_2_disabledYearAndMonth() {
            var date = new Date("2013/12/1");
            var minDate = new Date("2013/12/1");
            var maxDate = new Date("2013/12/31");
            picker.minimum = minDate;
            picker.maximum = maxDate;
            picker.date = date;

            var yearPicker = findChild(picker, "DatePicker_YearPicker");
            compare(yearPicker.enabled, false, "year picker should be disabled");
            var monthPicker = findChild(picker, "DatePicker_MonthPicker");
            compare(monthPicker.enabled, false, "month picker should be disabled");
        }
    }
}

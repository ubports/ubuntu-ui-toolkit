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

    Component {
        id: testComponent
        DatePicker {
            width: testSuite.width
        }
    }
    Column {
        DatePicker {
            id: defaults
            width: testSuite.width
        }
        Loader {
            id: pickerLoader
            width: parent.width
        }
    }

    UbuntuTestCase {
        name: "DatePickerAPI"
        when: windowShown

        readonly property DatePicker picker: pickerLoader.item

        function loadTest() {
            pickerLoader.sourceComponent = testComponent;
            tryCompareFunction(function(){return pickerLoader.status}, Loader.Ready);
        }
        function cleanup() {
            pickerLoader.sourceComponent = undefined;
        }

        function getPickerLabel(picker, name) {
            var pickerItem = findChild(picker, name);
            var pickerCurrent = findChild(pickerItem, "Picker_ViewLoader");
            return findChild(pickerCurrent.item.currentItem, "PickerRow_PickerLabel");
        }
        function getPickerModel(picker, name) {
            var pickerItem = findChild(picker, name);
            return pickerItem ? pickerItem.model : undefined;
        }
        function setHMS(date, h, m, s) {
            date.setHours(h);
            date.setMinutes(m);
            date.setSeconds(s);
            return date;
        }

        function test_0_mode() {
            compare(defaults.mode, "Years|Months|Days", "default mode");
        }
        function test_0_date() {
            var date = (new Date()).midnight();
            compare(defaults.date, date, "default mode");
            compare(defaults.year, date.getFullYear(), "default year");
            compare(defaults.month, date.getMonth(), "default month");
            compare(defaults.day, date.getDate(), "default day");
            compare(defaults.week, date.getWeek(), "default week");
            compare(defaults.hours, date.getHours(), "default hour");
            compare(defaults.minutes, date.getMinutes(), "default minute");
            compare(defaults.seconds, date.getSeconds(), "default second");
        }
        function test_0_minimum_maximum() {
            var endDate = Date.prototype.midnight.call(new Date());
            endDate.setFullYear(endDate.getFullYear() + 50);
            compare(defaults.minimum, defaults.date, "default minimum is same as date property");
            compare(defaults.maximum, endDate, "default maximum is a 50 years window");
        }
        function test_0_locale() {
            compare(defaults.locale, Qt.locale(), "default locale is the same as system locale");
        }

        function test_1_changeModeYM() {
            loadTest()
            var newMode = "Years|Months";
            var pickerCount = 2 + 1; // +1 is the Repeater
            picker.mode = newMode;
            tryCompareFunction(function(){return picker.moving}, false);
            var positioner = findChild(picker, "PickerRow_Positioner");
            compare(positioner.children.length, pickerCount, "invalid amount of pickers");

            cleanup();
        }

        function test_1_changeModeMD() {
            loadTest();
            var newMode = "Days|Months";
            var pickerCount = 2 + 1; // +1 is the Repeater
            picker.mode = newMode;
            tryCompareFunction(function(){return picker.moving}, false);
            var positioner = findChild(picker, "PickerRow_Positioner");
            compare(positioner.children.length, pickerCount, "invalid amount of pickers");
            cleanup();
        }

        function test_1_changeModeYD() {
            loadTest();
            var newMode = "Years|Days";
            var pickerCount = 2 + 1; // +1 is the Repeater
            picker.mode = newMode;
            tryCompareFunction(function(){return picker.moving}, false);
            var positioner = findChild(picker, "PickerRow_Positioner");
            expectFailContinue("", "Invalid mode");
            compare(positioner.children.length, pickerCount, "invalid amount of pickers");
            cleanup();
        }

        function test_1_changeModeY() {
            loadTest();
            var newMode = "Years";
            var pickerCount = 1 + 1; // +1 is the Repeater
            picker.mode = newMode;
            // FIXME: find out why tryCompareFunction blocks
            wait(500);
            var positioner = findChild(picker, "PickerRow_Positioner");
            compare(positioner.children.length, pickerCount, "invalid amount of pickers");
            cleanup();
        }

        function test_1_changeModeM() {
            loadTest();
            var newMode = "Months";
            var pickerCount = 1 + 1; // +1 is the Repeater
            picker.mode = newMode;
            tryCompareFunction(function(){return picker.moving}, false);
            var positioner = findChild(picker, "PickerRow_Positioner");
            compare(positioner.children.length, pickerCount, "invalid amount of pickers");
            cleanup();
        }

        function test_1_changeModeD() {
            loadTest();
            var newMode = "Days";
            var pickerCount = 1 + 1; // +1 is the Repeater
            picker.mode = newMode;
            tryCompareFunction(function(){return picker.moving}, false);
            var positioner = findChild(picker, "PickerRow_Positioner");
            compare(positioner.children.length, pickerCount, "invalid amount of pickers");
            cleanup();
        }

        function test_1_changeModeHMS() {
            loadTest();
            var newMode = "Hours|Minutes|Seconds";
            var pickerCount = 3 + 1; // +1 is the Repeater
            picker.mode = newMode;
            // FIXME: find out why tryCompareFunction blocks
            wait(500);
            var positioner = findChild(picker, "PickerRow_Positioner");
            compare(positioner.children.length, pickerCount, "invalid amount of pickers");
            cleanup();
        }

        function test_1_changeModeHM() {
            loadTest();
            var newMode = "Hours|Minutes";
            var pickerCount = 2 + 1; // +1 is the Repeater
            picker.mode = newMode;
            // FIXME: find out why tryCompareFunction blocks
            wait(500);
            var positioner = findChild(picker, "PickerRow_Positioner");
            compare(positioner.children.length, pickerCount, "invalid amount of pickers");
            cleanup();
        }

        function test_1_changeModeMS() {
            loadTest();
            var newMode = "Minutes|Seconds";
            var pickerCount = 2 + 1; // +1 is the Repeater
            picker.mode = newMode;
            // FIXME: find out why tryCompareFunction blocks
            wait(500);
            var positioner = findChild(picker, "PickerRow_Positioner");
            compare(positioner.children.length, pickerCount, "invalid amount of pickers");
            cleanup();
        }

        function test_1_changeModeHS() {
            loadTest();
            var newMode = "Hours|Seconds";
            var pickerCount = 2 + 1; // +1 is the Repeater
            picker.mode = newMode;
            tryCompareFunction(function(){return picker.moving}, false);
            var positioner = findChild(picker, "PickerRow_Positioner");
            expectFailContinue("", "cannot set mode to Hours|Minutes");
            compare(positioner.children.length, pickerCount, "invalid amount of pickers");
            cleanup();
        }

        function test_1_changeModeH() {
            loadTest();
            var newMode = "Hours";
            var pickerCount = 1 + 1; // +1 is the Repeater
            picker.mode = newMode;
            // FIXME: find out why tryCompareFunction blocks
            wait(500);
            var positioner = findChild(picker, "PickerRow_Positioner");
            compare(positioner.children.length, pickerCount, "invalid amount of pickers");
            cleanup();
        }

        function test_1_changeModeMinute() {
            loadTest();
            var newMode = "Minutes";
            var pickerCount = 1 + 1; // +1 is the Repeater
            picker.mode = newMode;
            // FIXME: find out why tryCompareFunction blocks
            wait(500);
            var positioner = findChild(picker, "PickerRow_Positioner");
            compare(positioner.children.length, pickerCount, "invalid amount of pickers");
            cleanup();
        }

        function test_1_changeModeS() {
            loadTest();
            var newMode = "Seconds";
            var pickerCount = 1 + 1; // +1 is the Repeater
            picker.mode = newMode;
            // FIXME: find out why tryCompareFunction blocks
            wait(500);
            var positioner = findChild(picker, "PickerRow_Positioner");
            compare(positioner.children.length, pickerCount, "invalid amount of pickers");
            cleanup();
        }

        function test_1_changeModeYMDHMS() {
            loadTest();
            var newMode = "Years|Months|Days|Hours|Minutes|Seconds";
            var pickerCount = 6 + 1; // +1 is the Repeater
            picker.mode = newMode;
            tryCompareFunction(function(){return picker.moving}, false);
            var positioner = findChild(picker, "PickerRow_Positioner");
            expectFailContinue("", "cannot combine date and time pickers");
            compare(positioner.children.length, pickerCount, "invalid amount of pickers");
            cleanup();
        }

        function test_1_changeModeYH() {
            loadTest();
            var newMode = "Years|Hours";
            var pickerCount = 2 + 1; // +1 is the Repeater
            picker.mode = newMode;
            tryCompareFunction(function(){return picker.moving}, false);
            var positioner = findChild(picker, "PickerRow_Positioner");
            expectFailContinue("", "cannot combine date and time pickers");
            compare(positioner.children.length, pickerCount, "invalid amount of pickers");
            cleanup();
        }

        function test_1_changeModeUnhandled() {
            loadTest();
            var newMode = "Years|Whatever";
            var pickerCount = 2 + 1; // +1 is the Repeater
            picker.mode = newMode;
            tryCompareFunction(function(){return picker.moving}, false);
            var positioner = findChild(picker, "PickerRow_Positioner");
            expectFailContinue("", "unhandled mode flag should not pass");
            compare(positioner.children.length, pickerCount, "invalid amount of pickers");
            cleanup();
        }

        function test_1_changeLocale() {
            loadTest();
            var prevLocale = picker.locale;
            var locale = Qt.locale("hu_HU");
            picker.minimum = new Date(2012, 11, 1);
            picker.date = new Date(2012, 11, 1);
            tryCompareFunction(function(){return picker.moving}, false);
            picker.locale = Qt.locale("hu_HU");
            tryCompareFunction(function(){return picker.moving}, false);
            var label = getPickerLabel(picker, "PickerRow_MonthPicker");
            compare(label.text, locale.monthName(picker.date.getMonth(), Locale.LongFormat), "locale for month wrong");

            label = getPickerLabel(picker, "PickerRow_DayPicker");
            var dayModel = getPickerModel(picker, "PickerRow_DayPicker");
            compare(label.text, dayModel.text(picker.date.getDate() - 1, testSuite.width), "locale for day name wrong");
            cleanup();
        }

        // FIXME: flaky
        function test_1_changeMinimumBeforeDate() {
            loadTest();
            var date = new Date(picker.date);
            var originalDate = new Date(date);
            date.setFullYear(date.getFullYear() - 1);
            date.setDate(1);
            picker.minimum = date;
            tryCompareFunction(function(){return picker.moving}, false);
            var year = getPickerLabel(picker, "PickerRow_YearPicker");
            compare(year.text, originalDate.getFullYear().toString(), "year differs");
            var month = getPickerLabel(picker, "PickerRow_MonthPicker");
            compare(month.text, picker.locale.monthName(originalDate.getMonth(), Locale.LongFormat), "month differs");
            var day = getPickerLabel(picker, "PickerRow_DayPicker");
            var dayModel = getPickerModel(picker, "PickerRow_DayPicker");
            compare(day.text, dayModel.text(originalDate.getDate() - 1), "day differs");
            cleanup();
        }

        // FIXME: flaky
        function test_1_changeMaximumAfterDate() {
            loadTest();
            var date = new Date(picker.date);
            var originalDate = new Date(date);
            date.setFullYear(date.getFullYear() + 1);
            date.setDate(1);
            picker.maximum = date;
            tryCompareFunction(function(){return picker.moving}, false);
            var year = getPickerLabel(picker, "PickerRow_YearPicker");
            compare(year.text, originalDate.getFullYear().toString(), "year differs");
            var month = getPickerLabel(picker, "PickerRow_MonthPicker");
            compare(month.text, picker.locale.monthName(originalDate.getMonth(), Locale.LongFormat), "month differs");
            var day = getPickerLabel(picker, "PickerRow_DayPicker");
            var dayModel = getPickerModel(picker, "PickerRow_DayPicker");
            compare(day.text, dayModel.text(originalDate.getDate() - 1), "day differs");
            cleanup();
        }

        // make infinite
        function test_1_changeMinimumInvalid() {
            loadTest();
            picker.minimum = Date.prototype.getInvalidDate.call();
            tryCompareFunction(function(){return picker.moving}, false);
            compare(picker.minimum, picker.date, "invalid minimum hasn't been adjusted to date");
            cleanup();
        }

        // make infinite
        function test_1_changeMaximumInvalid() {
            loadTest();
            picker.maximum = Date.prototype.getInvalidDate.call();
            tryCompareFunction(function(){return picker.moving}, false);
            // check if the year picker model is autoExtending
            var yearModel = getPickerModel(picker, "PickerRow_YearPicker");
            compare(yearModel.autoExtend, true, "the year picker is not auto-extending one");
            cleanup();
        }

        function test_1_changeDate() {
            loadTest();
            var date = picker.date;
            date.setFullYear(date.getFullYear() + 2);
            date.setMonth(5);
            date.setDate(21);
            picker.date = date;
            picker.mode = "Years|Months|Days";
            tryCompareFunction(function(){return picker.moving}, false);

            var yearLabel = getPickerLabel(picker, "PickerRow_YearPicker");
            var monthLabel = getPickerLabel(picker, "PickerRow_MonthPicker");
            var monthModel = getPickerModel(picker, "PickerRow_MonthPicker");
            var dayLabel = getPickerLabel(picker, "PickerRow_DayPicker");
            var dayModel = getPickerModel(picker, "PickerRow_DayPicker");
            compare(yearLabel.text, date.getFullYear().toString(), "different year value");
            compare(monthLabel.text, monthModel.text(date.getMonth()), "different month value");
            compare(dayLabel.text, dayModel.text(date.getDate() - 1), "different day value");
            cleanup();
        }

        function test_1_changeDateToNextMonth() {
            loadTest();
            picker.minimum = new Date(2013, 9, 1);
            picker.date = new Date(2013, 09, 31);
            picker.locale = Qt.locale("hu_HU")
            tryCompareFunction(function(){return picker.moving}, false);

            // click on the month picker to set the next month
            var monthPicker = findChild(picker, "PickerRow_MonthPicker");
            var monthCurrent = findChild(monthPicker, "Picker_ViewLoader");
            var my = monthPicker.y + (monthPicker.height / 2) + monthCurrent.item.currentItem.height;
            var mx = monthPicker.x + monthPicker.width / 2;
            mouseClick(picker, mx, my);
            tryCompareFunction(function(){return picker.moving}, false);

            var yearLabel = getPickerLabel(picker, "PickerRow_YearPicker");
            var monthLabel = getPickerLabel(picker, "PickerRow_MonthPicker");
            var monthModel = getPickerModel(picker, "PickerRow_MonthPicker");
            var dayLabel = getPickerLabel(picker, "PickerRow_DayPicker");
            var dayModel = getPickerModel(picker, "PickerRow_DayPicker");
            compare(yearLabel.text, "2013", "different year value");
            // November
            compare(monthLabel.text, monthModel.text(10), "different month value");
            // the 30th
            compare(dayLabel.text, dayModel.text(29), "different day value");

            // set it back
            my = monthPicker.y + (monthPicker.height / 2) - monthCurrent.item.currentItem.height;
            mouseClick(picker, mx, my);
            tryCompareFunction(function(){return picker.moving}, false);

            compare(yearLabel.text, "2013", "different year value");
            // October
            monthLabel = getPickerLabel(picker, "PickerRow_MonthPicker");
            compare(monthLabel.text, monthModel.text(9), "different month value");
            // the 30th
            dayLabel = getPickerLabel(picker, "PickerRow_DayPicker");
            compare(dayLabel.text, dayModel.text(29), "different day value");
            cleanup();
        }

        function test_2_disabledYear() {
            loadTest();
            var date = new Date(2013, 4, 1);
            var minDate = new Date(2013, 0, 1);
            var maxDate = new Date(2013, 11, 31);
            picker.minimum = minDate;
            picker.maximum = maxDate;
            tryCompareFunction(function(){return picker.moving}, false);
            picker.date = date;
            tryCompareFunction(function(){return picker.moving}, false);

            var yearPicker = findChild(picker, "PickerRow_YearPicker");
            compare(yearPicker.enabled, false, "year picker should be disabled");
            cleanup();
        }

        function test_2_disabledYearAndMonth() {
            loadTest();
            var date = new Date(2013, 11, 1);
            var minDate = new Date(2013, 11, 1);
            var maxDate = new Date(2013, 11, 31);
            picker.minimum = minDate;
            picker.maximum = maxDate;
            picker.date = date;
            tryCompareFunction(function(){return picker.moving}, false);

            var yearPicker = findChild(picker, "PickerRow_YearPicker");
            compare(yearPicker.enabled, false, "year picker should be disabled");
            var monthPicker = findChild(picker, "PickerRow_MonthPicker");
            compare(monthPicker.enabled, false, "month picker should be disabled");
            cleanup();
        }

        function test_2_linearDayPicker() {
            loadTest();
            var date = new Date(2013, 11, 1);
            var minDate = new Date(2013, 11, 2);
            var maxDate = new Date(2013, 11, 31);
            picker.minimum = minDate;
            picker.maximum = maxDate;
            picker.date = date;

            var yearPicker = findChild(picker, "PickerRow_YearPicker");
            compare(yearPicker.enabled, false, "year picker should be disabled");
            var monthPicker = findChild(picker, "PickerRow_MonthPicker");
            compare(monthPicker.enabled, false, "month picker should be disabled");
            var dayPickerModel = getPickerModel(picker, "PickerRow_DayPicker");
            expectFailContinue("", "day picker is always circular");
            compare(dayPickerModel.circular, false, "day picker should be linear");
            cleanup();
        }

        function test_3_changeHours() {
            loadTest();
            picker.mode = "Hours|Minutes|Seconds";
            var date = new Date(picker.date);
            date.setHours((date.getHours() + 10) % 24);
            picker.date = date;
            tryCompareFunction(function(){return picker.moving}, false);

            var hoursLabel = getPickerLabel(picker, "PickerRow_HoursPicker");
            verify(hoursLabel, "hour label undefined");
            compare(hoursLabel.text, ("00" + date.getHours()).slice(-2), "hours differ");
            var minutesLabel = getPickerLabel(picker, "PickerRow_MinutesPicker");
            verify(minutesLabel, "minutes label undefined");
            compare(minutesLabel.text, ("00" + date.getMinutes()).slice(-2), "minutes differ");
            var secondsLabel = getPickerLabel(picker, "PickerRow_SecondsPicker");
            verify(secondsLabel, "seconds label undefined");
            compare(secondsLabel.text, ("00" + date.getSeconds()).slice(-2), "seconds differ");
            cleanup();
        }

//        function test_3_changeMinutes() {
//            loadTest();
//            picker.mode = "Hours|Minutes|Seconds";
//            var date = new Date(picker.date);
//            date.setMinutes((date.getMinutes() + 40) % 60);
//            picker.date = date;
//            tryCompareFunction(function(){return picker.moving}, false);

//            var hoursLabel = getPickerLabel(picker, "PickerRow_HoursPicker");
//            verify(hoursLabel, "hour label undefined");
//            compare(hoursLabel.text, ("00" + date.getHours()).slice(-2), "hours differ");
//            var minutesLabel = getPickerLabel(picker, "PickerRow_MinutesPicker");
//            verify(minutesLabel, "minutes label undefined");
//            compare(minutesLabel.text, ("00" + date.getMinutes()).slice(-2), "minutes differ");
//            var secondsLabel = getPickerLabel(picker, "PickerRow_SecondsPicker");
//            verify(secondsLabel, "seconds label undefined");
//            compare(secondsLabel.text, ("00" + date.getSeconds()).slice(-2), "seconds differ");
//            cleanup();
//        }

//        function test_3_changeSeconds() {
//            loadTest();
//            picker.mode = "Hours|Minutes|Seconds";
//            var date = new Date(picker.date);
//            date.setSeconds((date.getSeconds() + 50) % 60);
//            picker.date = date;
//            tryCompareFunction(function(){return picker.moving}, false);

//            var hoursLabel = getPickerLabel(picker, "PickerRow_HoursPicker");
//            verify(hoursLabel, "hour label undefined");
//            compare(hoursLabel.text, ("00" + date.getHours()).slice(-2), "hours differ");
//            var minutesLabel = getPickerLabel(picker, "PickerRow_MinutesPicker");
//            verify(minutesLabel, "minutes label undefined");
//            compare(minutesLabel.text, ("00" + date.getMinutes()).slice(-2), "minutes differ");
//            var secondsLabel = getPickerLabel(picker, "PickerRow_SecondsPicker");
//            verify(secondsLabel, "seconds label undefined");
//            compare(secondsLabel.text, ("00" + date.getSeconds()).slice(-2), "seconds differ");
//            cleanup();
//        }

//        function test_4_changeMinimumBeforeDateHMS() {
//            loadTest();
//            picker.mode = "Hours|Minutes|Seconds";
//            var date = new Date(picker.date);
//            var originalDate = new Date(date);
//            date.setFullYear(date.getFullYear() - 1);
//            date.setDate(1);
//            picker.minimum = date;
//            tryCompareFunction(function(){return picker.moving}, false);

//            var hoursLabel = getPickerLabel(picker, "PickerRow_HoursPicker");
//            verify(hoursLabel, "hour label undefined");
//            compare(hoursLabel.text, ("00" + originalDate.getHours()).slice(-2), "hours differ");
//            var minutesLabel = getPickerLabel(picker, "PickerRow_MinutesPicker");
//            verify(minutesLabel, "minutes label undefined");
//            compare(minutesLabel.text, ("00" + originalDate.getMinutes()).slice(-2), "minutes differ");
//            var secondsLabel = getPickerLabel(picker, "PickerRow_SecondsPicker");
//            verify(secondsLabel, "seconds label undefined");
//            compare(secondsLabel.text, ("00" + originalDate.getSeconds()).slice(-2), "seconds differ");
//            cleanup();
//        }

//        function test_4_changeMaximumAfterDateHMS() {
//            loadTest();
//            picker.mode = "Hours|Minutes|Seconds";
//            var date = new Date(picker.date);
//            var originalDate = new Date(date);
//            date.setFullYear(date.getFullYear() + 1);
//            date.setDate(1);
//            picker.maximum = date;
//            tryCompareFunction(function(){return picker.moving}, false);
//            var hoursLabel = getPickerLabel(picker, "PickerRow_HoursPicker");
//            verify(hoursLabel, "hour label undefined");
//            compare(hoursLabel.text, ("00" + originalDate.getHours()).slice(-2), "hours differ");
//            var minutesLabel = getPickerLabel(picker, "PickerRow_MinutesPicker");
//            verify(minutesLabel, "minutes label undefined");
//            compare(minutesLabel.text, ("00" + originalDate.getMinutes()).slice(-2), "minutes differ");
//            var secondsLabel = getPickerLabel(picker, "PickerRow_SecondsPicker");
//            verify(secondsLabel, "seconds label undefined");
//            compare(secondsLabel.text, ("00" + originalDate.getSeconds()).slice(-2), "seconds differ");
//            cleanup();
//        }

//        function test_4_disabledHour() {
//            loadTest();
//            picker.mode = "Hours|Minutes|Seconds";
//            var date = setHMS(new Date(), 12, 10, 45);
//            var minDate = setHMS(new Date(), 12, 0, 0);
//            var maxDate = setHMS(new Date(), 12, 59, 59);
//            picker.minimum = minDate;
//            picker.maximum = maxDate;
//            tryCompareFunction(function(){return picker.moving}, false);
//            picker.date = date;

//            var hoursPicker = findChild(picker, "PickerRow_HoursPicker");
//            compare(hoursPicker.enabled, false, "hours picker should be disabled");
//            cleanup();
//        }

//        function test_4_disabledHoursAndMinutes() {
//            loadTest();
//            picker.mode = "Hours|Minutes|Seconds";
//            var date = setHMS(new Date(), 12, 10, 45);
//            var minDate = setHMS(new Date(), 12, 10, 0);
//            var maxDate = setHMS(new Date(), 12, 10, 59);
//            picker.minimum = minDate;
//            picker.maximum = maxDate;
//            picker.date = date;
//            tryCompareFunction(function(){return picker.moving}, false);

//            var hoursPicker = findChild(picker, "PickerRow_HoursPicker");
//            compare(hoursPicker.enabled, false, "hours picker should be disabled");
//            var minutesPicker = findChild(picker, "PickerRow_MinutesPicker");
//            compare(minutesPicker.enabled, false, "minutes picker should be disabled");
//            cleanup();
//        }

//        function test_4_linearSecondsPicker() {
//            loadTest();
//            picker.mode = "Hours|Minutes|Seconds";
//            var date = setHMS(new Date(), 12, 10, 45);
//            var minDate = setHMS(new Date(), 12, 10, 1);
//            var maxDate = setHMS(new Date(), 12, 10, 59);
//            picker.minimum = minDate;
//            picker.maximum = maxDate;
//            picker.date = date;
//            tryCompareFunction(function(){return picker.moving}, false);

//            var hoursPicker = findChild(picker, "PickerRow_HoursPicker");
//            compare(hoursPicker.enabled, false, "hours picker should be disabled");
//            var minutesPicker = findChild(picker, "PickerRow_MinutesPicker");
//            compare(minutesPicker.enabled, false, "minutes picker should be disabled");
//            var secondsPickerModel = getPickerModel(picker, "PickerRow_SecondsPicker");
//            compare(secondsPickerModel.circular, false, "day picker should be linear");
//            cleanup();
//        }

    }
}

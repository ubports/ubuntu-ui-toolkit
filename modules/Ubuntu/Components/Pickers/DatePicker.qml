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
import "dateUtils.js" as DateUtils
import "../" 0.1

Rectangle {
    id: datePicker

    /*!
      */
    property date date: new Date()

    /*!
      \qmlproperty int minimumYear
      \qmlproperty int maximumYear
      */
    property int minimumYear: internals.fromYear
    /*! \internal */
    property int maximumYear: internals.toYear

    /*!
      */
    property Component pickerStyle

    implicitWidth: units.gu(40)
    implicitHeight: {
        var h = Qt.inputMethod.keyboardRectangle.height;
        if (h > 0) {
            return h;
        }
        return units.gu(20);
    }

    // component to calculate text fitting
    Label { id: textSizer; visible: false }
    // tumblers
    Row {
        anchors.fill: parent
        Picker {
            id: yearPicker
            circular: false
            model: ListModel{}
            width: limits.minimumWidth()
            delegate: PickerDelegate {
                Label {
                    text: modelData
                    anchors {
                        verticalCenter: parent.verticalCenter
                        horizontalCenter: parent.horizontalCenter
                    }
                }
                Component.onCompleted: {
                    if (index === (yearPicker.model.count - 1))
                        internals.updateYearModel(modelData + 1, modelData + 50);
                }
            }
        }
        Picker {
            id: monthPicker
            model: [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11]
            width: (parent.width - yearPicker.width - dayPicker.width) > limits.minimumWidth() ?
                       (parent.width - yearPicker.width - dayPicker.width) : limits.minimumWidth()
            delegate: PickerDelegate {
                Label {
                    text: DateUtils.monthText(date, modelData, limits.monthPickerFormat())
                    anchors{
                        verticalCenter: parent.verticalCenter
                        left: parent.left
                        leftMargin: units.gu(0.5)
                    }
                }
            }
        }
        Picker {
            id: dayPicker
            property int limitIndex: limits.dayPickerLimitIndex()
            width: limits.dayPickerWidth(limitIndex)
            model: ListModel{}
            delegate: PickerDelegate {
                Label {
                    text: DateUtils.dayText(date, modelData + 1, limits.dayPickerFormat(dayPicker.limitIndex))
                    anchors{
                        verticalCenter: parent.verticalCenter
                        left: parent.left
                        leftMargin: units.gu(0.5)
                    }
                }
            }
        }
    }

    /* \internal */
    onMinimumYearChanged: internals.updateYearModel(minimumYear, maximumYear)
    /* \internal */
    onMaximumYearChanged: internals.updateYearModel(minimumYear, maximumYear)
    /*! \internal */
    onWidthChanged: {
        // clamp to 3 times the minimum Picker width
        if (width < 3 * limits.get(0).dayPickerWidth)
            width = 3 * limits.get(0).dayPickerWidth;
    }

    Component.onCompleted: {
        internals.updateModels();
        internals.completed = true;
    }

    ListModel {
        id: limits
        ListElement {
            dayPickerWidthLimit: 7.3
            dayPickerWidth: 6
            monthPickerLimit: 10
            textFormat: "narrow"
        }
        ListElement {
            dayPickerWidthLimit: 11.5
            dayPickerWidth: 8
            monthPickerLimit: 15
            textFormat: "short"
        }
        ListElement {
            dayPickerWidthLimit: 13.5
            dayPickerWidth: 13.5
            monthPickerLimit: 0
            textFormat: "long"
        }

        Component.onCompleted: {
            get(count - 1).monthPickerLimit = datePicker.width
        }

        function minimumWidth() {
            return units.gu(get(0).dayPickerWidth);
        }

        function dayPickerLimitIndex() {
            var w = (datePicker.width - yearPicker.width) * 40 / 100;
            for (var i = 0; i < count; i++) {
                if (w < units.gu(get(i).dayPickerWidthLimit)) {
                    return i;
                }
            }
            return -1;
        }
        function dayPickerWidth(index) {
            if (index < 0) {
                return (datePicker.width - yearPicker.width) * 40 / 100;
            }
            return units.gu(get(index).dayPickerWidth);
        }
        function dayPickerFormat(index) {
            return (index < 0) ? get(count - 1).textFormat : get(index).textFormat;

        }

        function monthPickerFormat() {
            for (var i = 0; i < count; i++) {
                if (monthPicker.width < units.gu(get(i).monthPickerLimit)) {
                    return get(i).textFormat;
                }
            }
            return "";
        }
    }

    QtObject {
        id: internals
        property bool completed: false
        property int fromYear: (minimumYear <= 0) ? datePicker.date.getFullYear() : minimumYear;
        property int toYear: 0
        readonly property int year: datePicker.date.getFullYear()
        readonly property int month: datePicker.date.getMonth()
        readonly property int day: datePicker.date.getDate();
        property int yearIndex: yearPicker.selectedIndex
        property int monthIndex: monthPicker.selectedIndex
        property int dayIndex: dayPicker.selectedIndex

        onYearIndexChanged: {
            if (!completed) return;
            datePicker.date = DateUtils.updateYear(datePicker.date, fromYear + yearPicker.selectedIndex);
            updateDayModel(ear, month);
        }
        onMonthIndexChanged: {
            if (!completed) return;
            datePicker.date = DateUtils.updateMonth(datePicker.date, monthPicker.selectedIndex)
            updateDayModel(year, month);
        }
        onDayIndexChanged: {
            if (!completed) return;
            datePicker.date = DateUtils.updateDay(datePicker.date, dayPicker.selectedIndex + 1);
        }

        function updateModels() {
            updateYearModel(fromYear, toYear);
            monthPicker.selectedIndex = month;
            updateDayModel(year, month);
        }
        function updateYearModel(from, to) {
            if (from <= 0) {
                from = year;
            }
            var max = (to >= from) ? to : from + 50;
            for (var i = from; i < max; i++) {
                yearPicker.model.append({"year": i});
            }
            yearPicker.selectedIndex = year - fromYear;
        }
        function updateDayModel(year, month) {
            if (!completed) {
                dayPicker.model.clear();
                for (var i = 0; i < DateUtils.daysInMonth(year, month); i++) {
                    dayPicker.model.append({"day": i});
                }
                dayPicker.selectedIndex = day - 1;
            } else {
                var newDaysCount = DateUtils.daysInMonth(year, month);
                var modelCount = dayPicker.model.count;
                var daysDiff = newDaysCount - modelCount;
                if (daysDiff < 0) {
                    dayPicker.model.remove(modelCount + daysDiff, -daysDiff);
                } else if (daysDiff > 0) {
                    for (var d = modelCount; d < modelCount + daysDiff; d++) {
                        dayPicker.model.append({"day": d});
                    }
                }
            }
        }
    }
}



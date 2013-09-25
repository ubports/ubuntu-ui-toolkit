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

    Row {
        anchors.fill: parent
        Picker {
            id: yearPicker
            circular: false
            model: ListModel{}
            width: internals.minimumPickerWidth
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
            width: (parent.width - yearPicker.width - dayPicker.width) > internals.minimumPickerWidth ?
                       (parent.width - yearPicker.width - dayPicker.width) : internals.minimumPickerWidth
            delegate: PickerDelegate {
                Label {
                    text: {
                        if (monthPicker.width > units.gu(15)) {
                            var thisDate = new Date(date);
                            thisDate.setMonth(modelData);
                            return Qt.formatDate(thisDate, "MM ") + Qt.locale().standaloneMonthName(modelData, Locale.LongFormat);
                        } else if (monthPicker.width > units.gu(10)) {
                            return Qt.locale().standaloneMonthName(modelData, Locale.LongFormat);
                        }
                        return Qt.locale().standaloneMonthName(modelData, Locale.ShortFormat);
                    }
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
            width: internals.calculateDayPickerWidth()
            model: ListModel{}
            delegate: PickerDelegate {
                Label {
                    text: {
                        var thisDate = new Date(date);
                        thisDate.setDate(modelData + 1);
                        if (dayPicker.width > units.gu(13)) {
                            return Qt.formatDate(thisDate, "dd ") + Qt.locale().dayName(thisDate.getDay(), Locale.LongFormat);
                        } else if (dayPicker.width > units.gu(7)) {
                            return Qt.formatDate(thisDate, "dd ") + Qt.locale().dayName(thisDate.getDay(), Locale.ShortFormat);
                        }

                        return Qt.formatDate(thisDate, "dd");
                    }
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

    Component.onCompleted: {
        internals.updateModels();
        internals.completed = true;
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

        property real minimumPickerWidth: units.gu(6)
        property var dayPickerWidthLimits: [units.gu(7.3), units.gu(11.5), units.gu(13.5)]
        property var dayPickerWidths: [minimumPickerWidth, units.gu(8), units.gu(13.5)]

        onYearIndexChanged: {
            if (!completed) return;
            datePicker.date = DateUtils.updateYear(datePicker.date, fromYear + yearPicker.selectedIndex);
            updateDayModel(year, month);
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

        function calculateDayPickerWidth() {
            var w = (datePicker.width - yearPicker.width) * 40 / 100;
            for (var i = 0; i < dayPickerWidthLimits.length; i++) {
                if (w < dayPickerWidthLimits[i]) {
                    return dayPickerWidths[i];
                }
            }
            return w;
        }
    }
}



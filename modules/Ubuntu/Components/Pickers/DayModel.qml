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
import Ubuntu.Components 0.1

PickerModelBase {
    circular: true

    // ommit minimum and maximum date values
    function reset(date, minimum, maximum) {
        clear();
        for (var i = 0; i < date.daysInMonth(); i++) {
            append({"day": i});
        }

        // call the pickerItem reset
        pickerItem.resetPicker();
    }

    function resetLimits(label, margin) {
        label.text = '9999';
        narrowFormatLimit = label.paintedWidth + 2 * margin;
        shortFormatLimit = longFormatLimit = 0.0;
        for (var day = 1; day <= 7; day++) {
            label.text = '99 ' + compositPicker.locale.dayName(day, Locale.ShortFormat)
            shortFormatLimit = Math.max(label.paintedWidth + 2 * margin, shortFormatLimit);
            label.text = '99 ' + compositPicker.locale.dayName(day, Locale.LongFormat)
            longFormatLimit = Math.max(label.paintedWidth + 2 * margin, longFormatLimit);
        }
    }

    function syncModels() {
        var newDaysCount = compositPicker.date.daysInMonth(compositPicker.year, compositPicker.month);
        var modelCount = count;
        var daysDiff = newDaysCount - modelCount;
        if (daysDiff < 0) {
            remove(modelCount + daysDiff, -daysDiff);
        } else if (daysDiff > 0) {
            for (var d = modelCount; d < modelCount + daysDiff; d++) {
                append({"day": d});
            }
        }
    }

    function indexOf(date) {
        return date.getDate() - 1;
    }

    function dateFromIndex(date, index) {
        var newDate = new Date(date);
        newDate.setDate(index + 1);
        return newDate;
    }

    function text(date, value, width) {
        var thisDate = new Date(date);
        thisDate.setDate(value + 1);

        if (width >= longFormatLimit) {
            return Qt.formatDate(thisDate, "dd ") + compositPicker.locale.dayName(thisDate.getDay(), Locale.LongFormat);
        }

        if (width >= shortFormatLimit) {
            return Qt.formatDate(thisDate, "dd ") + compositPicker.locale.dayName(thisDate.getDay(), Locale.ShortFormat);
        }
        return Qt.formatDate(thisDate, "dd");
    }
}

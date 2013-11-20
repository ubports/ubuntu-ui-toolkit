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

ListModel {
    property bool completed
    property bool circular: true

    function reset(date) {
        clear();
        for (var i = 0; i < date.daysInMonth(); i++) {
            dayPicker.model.append({"day": i});
        }
    }

    function update(date) {
        print(1)
        var newDaysCount = date.daysInMonth(year, month);
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
        print("getIndexTo "+date.getDate())
        return date.getDate() - 1;
    }

    function dateFromModel(date, index) {
        var newDate = new Date(date);
        newDate.setDate(index + 1);
        return newDate;
    }

    function text(date, value, format, locale) {
        var thisDate = new Date(date);
        thisDate.setDate(value + 1);
        switch (format) {
        case "long":
            return Qt.formatDate(thisDate, "dd ") + locale.dayName(thisDate.getDay(), Locale.LongFormat);
        case "short":
            return Qt.formatDate(thisDate, "dd ") + locale.dayName(thisDate.getDay(), Locale.ShortFormat);
        default:
            return Qt.formatDate(thisDate, "dd");
        }
    }
}

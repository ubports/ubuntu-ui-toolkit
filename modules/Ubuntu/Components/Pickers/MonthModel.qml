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
    property string mode
    property int from
    property int to
    property int distance
    property bool circular: (mode === "Week") ? (distance >= 52) : (distance >= 11)
    property real minimumTextWidth: 0.0
    property real maximumTextWidth: 0.0

    function reset(date, minimum, maximum) {
        clear();
        var modelDate = new Date(date);
        modelDate.setDate(1);

        if (mode === "Week") {
            // from and to is 0-51 if the maximum is invalid
            if (!maximum.isValid()) {
                from = 0;
                to = 51;
                distance = 52;
            } else {
                // from and to come from minimum and maximum
                from = minimum.getWeek();
                to = maximum.getWeek();
                distance = Math.min(minimum.weeksTo(maximum), 52);
                print(from+"|"+to+"|"+distance)
            }

            for (var i = from; i <= from + distance; i++) {
                modelDate = date.midDateOfWeek(i);
                append({"week": modelDate.getWeek()});
            }
        } else {
            // mode != Week
            // if maximum is invalid, we have full model (12 months to show)
            distance = to = maximum.isValid() ? minimum.monthsTo(maximum) : 11;
            if (to < 0 || to > 11) to = 11;
            from = (to < 11) ? minimum.getMonth() : 0;
            // fill the model
            for (var i = from; i <= from + to; i++) {
                modelDate.setMonth(i);
                append({"month": modelDate.getMonth()});
            }
        }
    }

    function updateLimits(textSizer, locale) {
        maximumTextWidth = Number.MAX_VALUE;
        if (mode === "Week") {

        } else {
            for (var month = 0; month < 12; month++) {
                textSizer.text = locale.monthName(month, Locale.ShortFormat);
                minimumTextWidth = Math.max(textSizer.paintedWidth, minimumTextWidth);
                textSizer.text = locale.monthName(month, Locale.LongFormat);
                maximumTextWidth = Math.min(textSizer.paintedWidth, maximumTextWidth);
            }
        }
    }

    function indexOf(date) {
        return (mode === "Week") ? date.getWeek() - 1: date.getMonth() - from;
    }

    function dateFromModel(date, index) {
        if (mode === "Week") {
            // update date to the first day of the week
            return date.midDateOfWeek(index + 1);
        } else {
            var newDate = new Date(date);
            // check if the days are in the diff zone (29-31)
            var fromDay = newDate.getDate();
            // move the day to the 1st of the month so we don't overflow when setting the month
            newDate.setDate(1);
            newDate.setMonth(get(index).month);
            var maxDays = newDate.daysInMonth();
            // check whether the original day would overflow
            // and trim to the mont's maximum date
            newDate.setDate((fromDay > maxDays) ? maxDays : fromDay);
            return newDate;
        }
    }

    function text(date, value, format, locale) {
        print(locale)
        if (mode === "Week") {
            value++;
            var startDate = date.startDateOfWeek(value);
            var endDate = new Date(startDate);
            endDate.setDate(endDate.getDate() + 6);

            var text = "W" + (Math.pow(10, 2) + ~~value).toString().substring(1);
            var differentMonth = startDate.getMonth() !== endDate.getMonth();
            switch (format) {
            case "long":
                return text + Qt.formatDate(startDate, " MMMM d - ") +
                        (differentMonth ? Qt.formatDate(endDate, "MMMM d") : Qt.formatDate(endDate, "d"));
            case "short":
                return text + Qt.formatDate(startDate, " MMM dd - ") +
                        (differentMonth ? Qt.formatDate(endDate, "MMM d") : Qt.formatDate(endDate, "d"));
            default:
                return text + Qt.formatDate(startDate, " MMM d");
            }
        } else {
            switch (format) {
            case "long":
                var delegateDate = new Date(date);
                delegateDate.setDate(1);
                delegateDate.setMonth(value);
                return Qt.formatDate(delegateDate, "MM ") + locale.monthName(value, Locale.LongFormat);
            case "short":
                return locale.monthName(value, Locale.LongFormat);
            default:
                return locale.monthName(value, Locale.ShortFormat);
            }
        }
    }
}

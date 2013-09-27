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

.pragma library
.import QtQuick 2.0 as Quick

Date.msPerDay = 86400e3
Date.msPerWeek = Date.msPerDay * 7

function updateYear(date, year) {
    return new Date(year, date.getMonth(), date.getDate());
}

function updateMonth(date, month) {
    return new Date(date.getFullYear(), month, date.getDate());
}

function updateDay(date, day) {
    return new Date(date.getFullYear(), date.getMonth(), day);
}

function leapYear(year) {
    return year % 4 == 0 && (year % 100 != 0 || year % 400 == 0);
}

function daysInMonth(year, month) {
    return [
        31/*Jan*/, 28/*Feb*/, 31/*Mar*/, 30/*Apr*/, 31/*May*/, 30/*Jun*/,
        31/*Jul*/, 31/*Aug*/, 30/*Sep*/, 31/*Oct*/, 30/*Nov*/, 31/*Dec*/
    ][month] + (month === 1) * leapYear(year);
}

function weeksInMonth(year, month, weekday) {
    var y = year, m = month;
    var date0 = new Date(y, m, 1);
    var date1 = new Date(y + (m == 11), m < 11 ? m + 1 : 0, 1);
    var day = date0.getDay();
    var m = (date1.getTime() - date0.getTime()) / Date.msPerDay;
    var n = 0;
    while (m > 0) {
        if (day == weekday) n = n + 1;
        day = day < 6 ? day + 1 : 0;
        m = m - 1;
    }
    return n;
}

function weekNumber(date) {
    // Copy date so don't modify original
    date = new Date(date);
    date.setHours(0, 0, 0, 0);
    // Set to nearest Thursday: current date + 4 - current day number
    // Make Sunday's day number 7
    date.setDate(date.getDate() + 4 - (date.getDay() || 7));
    // Get first day of year
    var yearStart = new Date(date.getFullYear(), 0, 1);
    // Calculate full weeks to nearest Thursday
    var weekNo = Math.ceil((((date - yearStart) / 86400000) + 1) / 7);
    // Return array of year and week number
    return weekNo;
}

function startDateOfWeek(year, week) {
    var simple = new Date(year, 0, 1 + (week - 1) * 7);
    var dow = simple.getDay();
    var ISOweekStart = simple;
    if (dow <= 4)
        ISOweekStart.setDate(simple.getDate() - simple.getDay() + 1);
    else
        ISOweekStart.setDate(simple.getDate() + 8 - simple.getDay());
    return ISOweekStart;
}

function midDateOfWeek(year, week) {
    var date = startDateOfWeek(year, week);
    date.setDate(date.getDate() + 3);
    return date;
}

function monthText(date, day, format) {
    switch (format) {
    case "long":
        var thisDate = new Date(date);
        thisDate.setMonth(day);
        return Qt.formatDate(thisDate, "MM ") + Qt.locale().standaloneMonthName(day, Quick.Locale.LongFormat);
    case "short":
        return Qt.locale().standaloneMonthName(day, Quick.Locale.LongFormat);
    default:
        return Qt.locale().standaloneMonthName(day, Quick.Locale.ShortFormat);
    }
}

function dayText(date, month, format) {
    var thisDate = new Date(date);
    thisDate.setDate(month);
    switch (format) {
    case "long":
        return Qt.formatDate(thisDate, "dd ") + Qt.locale().dayName(thisDate.getDay(), Quick.Locale.LongFormat);
    case "short":
        return Qt.formatDate(thisDate, "dd ") + Qt.locale().dayName(thisDate.getDay(), Quick.Locale.ShortFormat);
    default:
        return Qt.formatDate(thisDate, "dd");
    }
}

function weekText(date, weekNo, format) {
    weekNo++;
    var startDate = startDateOfWeek(date.getFullYear(), weekNo);
    var endDate = new Date(startDate);
    endDate.setDate(endDate.getDate() + 6);

    var text = "W" + (Math.pow(10, 2) + ~~weekNo).toString().substring(1);
    var differentMonth = startDate.getMonth() !== endDate.getMonth();

    print("weekTumbler.format=" + format)
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
}

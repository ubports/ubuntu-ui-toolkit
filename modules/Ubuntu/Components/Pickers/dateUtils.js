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

function weeksInMonth(year, month, weekday) {
    var y = year, m = month;
    var date0 = new Date(y, m, 1);
    var date1 = new Date(y + (m == 11), m < 11 ? m + 1 : 0, 1);
    var day = date0.getDay();
    var m = (date1.getTime() - date0.getTime()) / Date.msPerDay;
    var n = 0;
    while (m > 0) {
        if (day === weekday) n = n + 1;
        day = day < 6 ? day + 1 : 0;
        m = m - 1;
    }
    return n;
}

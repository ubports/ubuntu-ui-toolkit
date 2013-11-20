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

/*
  Extending Date with few prototypes
  */
Date.msPerDay = 86400e3
Date.msPerWeek = Date.msPerDay * 7

Date.prototype.isValid = function() {
    return (this.getFullYear() > 0) && (this.getMonth() >= 0) && (this.getDate() > 0);
}

Date.prototype.monthsTo = function(target) {
    return target.getMonth() - this.getMonth() + (12 * (target.getFullYear() - this.getFullYear()));
}

Date.prototype.daysTo = function(target) {
    return !target.isValid() ? 0 : Math.ceil((target - this) / Date.msPerDay);
}

Date.prototype.weeksTo = function(target) {
    return Math.ceil(this.daysTo(target) / 7);
}

Date.prototype.getWeek = function() {
    // Copy date so don't modify original
    var date = new Date(this);
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

Date.prototype.daysInMonth = function() {
    return [
        31/*an*/, 28/*Feb*/, 31/*Mar*/, 30/*Apr*/, 31/*May*/, 30/*Jun*/,
        31/*Jul*/, 31/*Aug*/, 30/*Sep*/, 31/*Oct*/, 30/*Nov*/, 31/*Dec*/
    ][this.getMonth()] + (this.getMonth() === 1) * this.leapYear();
}

Date.prototype.leapYear = function() {
    var year = this.getFullYear();
    return year % 4 == 0 && (year % 100 != 0 || year % 400 == 0);
}

Date.prototype.startDateOfWeek = function(week) {
    var year = this.getFullYear();
    var simple = new Date(year, 0, 1 + (week - 1) * 7);
    var dow = simple.getDay();
    var ISOweekStart = simple;
    if (dow <= 4)
        ISOweekStart.setDate(simple.getDate() - simple.getDay() + 1);
    else
        ISOweekStart.setDate(simple.getDate() + 8 - simple.getDay());
    return ISOweekStart;
}

Date.prototype.midDateOfWeek = function(week) {
    var date = this.startDateOfWeek(week);
    date.setDate(date.getDate() + 3);
    return date;
}

// Calculate text format based on width fitting into the tumbler
// the contentType parameter can be "Months", "Weeks" or "Days"
function PickerLimits(label, contentType, margin, minimumWidth) {
    var stack = new Array(0);

    this.getTextSize = function(text) {
        label.text = text;
        return Math.max(label.paintedWidth + 2 * margin, minimumWidth);
    }

    this.index = function(width) {
        for (var i = 0; i < stack.length; i++) {
            if (width >= stack[i].width) {
                return i;
            }
        }
        return stack.length - 1;
    }

    this.format = function(width) {
        if (!stack.length) {
            return "narrow";
        }

        for (var i = 0; i < stack.length; i++) {
            if (width >= stack[i].width) {
                return stack[i].format;
            }
        }
        return "";
    }

    this.clampWidth = function (width) {
        if (!stack.length) {
            return width;
        }

        for (var i = 0; i < stack.length; i++) {
            if (width >= stack[i].width) {
                return stack[i].width;
            }
        }
        return stack[stack.length - 1].width;
    }

    switch (contentType) {
    case "Months":
        stack.push({width: this.getTextSize("99 September"), format: "long"})
        stack.push({width: this.getTextSize("September"), format: "short"})
        stack.push({width: this.getTextSize("Sep"), format: "narrow"})
        break;
    case "Weeks":
        stack.push({width: this.getTextSize("W99 September 99 - October 9"), format: "long"})
        stack.push({width: this.getTextSize("W99 Sep 99 - Oct 9"), format: "short"})
        stack.push({width: this.getTextSize("W99 Sep 99"), format: "narrow"})
        break;
    case "Days":
        stack.push({width: this.getTextSize("99 Wednesday"), format: "long"})
        stack.push({width: this.getTextSize("99 Wed"), format: "short"})
        stack.push({width: this.getTextSize("99"), format: "narrow"})
        break;
    }
}

// Calculate text format based on width fitting into the tumbler
// the contentType parameter can be "Years", "Months", "Weeks" or "Days"
function PickerSize(labelHelper, contentType, margin, proportion) {
    var min = 0.0;
    var max = 0.0;
    var stack = new Array(0);

    // the function returns the date string based on the amount can fit into
    // the size
    this.fitText = function(theDate, proposedSize) {
        // try the maximum text
        switch (contentType) {
        case "Years":
            return theDate.getFullyear();
        case "Months":

            break;
        case "Weeks":
            break;
        case "Days":
            break;
        default:
            return undefined;
        }
    }

    function monthFormat(format) {
        switch (format) {
        case Quick.Locale.NarrowFormat:
            return "MM";
        case Quick.Locale.ShortFormat:
            return "MMMM";
        default:
            return "MM MMMM";
        }
    }

    function monthText(month, format) {
        if (format === Quick.Locale.NarrowFormat) {
            return Quick.Qt.locale().monthName(month, Quick.Locale.ShortFormat);
        }
        return '99' + Quick.Qt.locale().monthName(month, format);
    }

    function monthSizes(format) {
        max = Number.MAX_VALUE;
        var date = new Date();
        for (var month = 0; month < 12; month++) {
            date.setMonth(month, 1);
            labelHelper.text = Quick.Qt.formatDate(date, monthFormat(Quick.Locale.NarrowFormat));
            print(labelHelper.text)
            min = Math.max(labelHelper.paintedWidth + 2 * margin, min);
            labelHelper.text = Quick.Qt.formatDate(date, monthFormat(Quick.Locale.LongFormat));
            print(labelHelper.text)
            max = Math.min(labelHelper.paintedWidth + 2 * margin, max);
        }
    }

    switch (contentType) {
    case "Years":
        labelHelper.text = '9999';
        min = max = labelHelper.paintedWidth + 2 * margin;
        break;
    case "Months":
        min = monthSizes(Quick.Locale.NarrowFormat);
        max = monthSizes(Quick.Locale.LongFormat);
        break;
    case "Weeks":
        break;
    case "Days":
        break;
    }
}

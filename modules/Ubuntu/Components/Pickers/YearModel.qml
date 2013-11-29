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
    // local properties
    property int from
    property int to
    circular: false
    autoExtend: true

    function reset(date, minimum, maximum) {
        clear();
        from = (minimum.getFullYear() <= 0) ? date.getFullYear() : minimum.getFullYear();
        to = (maximum < minimum) ? 0 : maximum.getFullYear();
        autoExtend = !maximum.isValid();
        extend(from, to - from);

        // call the pickerItem reset
        pickerItem.resetPicker();
    }

    function resetLimits(label, margin) {
        label.text = "9999";
        narrowFormatLimit = shortFormatLimit = longFormatLimit = label.paintedWidth + 2 * margin;
    }

    function extend(baseYear, items) {
        if (items === undefined || items <= 0) {
            items = 50;
        }
        for (var i = baseYear; i <= baseYear + items; i++) {
            append({"modelData" : i});
        }
    }

    function indexOf(date) {
        return date.getFullYear() - from;
    }

    function dateFromIndex(date, index) {
        var newDate = new Date(date);
        newDate.setFullYear(index + from);
        return newDate;
    }

    function text(date, value, width) {
        return value;
    }
}

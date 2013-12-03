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

/*
  Base model type for Date- and TimePickers
  */
ListModel {

    /*
      Holds the picker item. This is not the composit component, it is the individual
      picker for a sub-value.
      */
    property Item pickerItem

    /*
      Property holding the composit picker component.
      */
    property Item compositPicker

    /*
      The property holds the width of the picker tumbler to be set.
      */
    property real pickerWidth: 0

    /*
      Specifies whether the model is circular or not.
      */
    property bool circular: false

    /*
      The property specifies whenther the Picker should auto-extend the model.
      Typical for year model.
      */
    property bool autoExtend: false

    /*
      Narow, normal and long format limits.
      */
    property real narrowFormatLimit: 0.0
    property real shortFormatLimit: 0.0
    property real longFormatLimit: 0.0

    /*
      The function resets the model and the attached Picker specified in the item.
      The Picker must have a resetPicker() function available.
      */
    function reset() {}

    /*
      Function called by the Picker to re-calculate values for the limit properties.
      The locale is retrieved from pickerItem.
      */
    function resetLimits(label, margin) {}

    /*
      The function is called by the Picker component when a value gets selected to
      keep the pickers in sync.
      */
    function syncModels() {}

    /*
      The function extends the model starting from a certain \a baseValue.
      */
    function extend(baseValue) {}

    /*
      Returns the index of the value from the model.
      */
    function indexOf(date) {
        return -1;
    }

    /*
      Returns a Date object from the model's \a index, relative to the given \a date.
      */
    function dateFromIndex(date, index) {
        return new Date();
    }

    /*
      Returns the date string for the value relative to the date, which fits into the
      given width. Uses the locale from pickerItem to fetch the localized date string.
      */
    function text(date, value, width) {
        return "";
    }

    /*!
      Readonly properties to the composit picker's date properties
      */
    readonly property date date: compositPicker.date
    readonly property date minimum: compositPicker.minimum
    readonly property date maximum: compositPicker.maximum

    property bool pickerCompleted: false

    /*
      Call reset() whenever minimum or maximum changes
      */
    onMinimumChanged: if (pickerCompleted) reset()
    onMaximumChanged: if (pickerCompleted) reset()

}

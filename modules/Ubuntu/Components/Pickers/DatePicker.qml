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

/*!
    \qmltype DatePicker
    \inqmlmodule Ubuntu.Components.Pickers 0.1
    \ingroup ubuntu-pickers
    \brief DatePicker component provides date value picking functionality.

    DatePicker combines up to three Picker elements providing different date value
    selection possibilities. It can be used to select full date (year, month, day)
    as well as to select a combination of year and month, month and day, or individual
    date units (i.e. year, month or day). The selected date as well as the initial
    one is provided by the \l date property. For convenience the component provides
    also the \a year, \a month, \a day and \a week values as separate properties,
    however these properties are not writable, and their initialization can happen
    only through the \l date property.

    \qml
    import QtQuick 2.0
    import Ubuntu.Components 0.1
    import Ubuntu.Components.Pickers 0.1

    Column {
        Label {
            text: "Selected date: W" + datePicker.week + " - " +
                    Qt.formatDate(datePicker.date, "dddd, dd-mmmm-yyyy")
        }
        DatePicker {
            id: datePicker
        }
    }
    \endqml

    The \l mode property specifies what date units should be shown by the picker.
    The property holds a string, combining \b Year, \b Month and \b Day strings
    or their first letter sepatared with '|' character. A DatePicker which shows
    only year and month date units would look as follows:
    \qml
    import QtQuick 2.0
    import Ubuntu.Components 0.1
    import Ubuntu.Components.Pickers 0.1

    Column {
        Label {
            text: "Selected month: " + Qt.formatDate(datePicker.date, "mmmm-yyyy")
        }
        DatePicker {
            id: datePicker
            mode: "Year|Month"
        }
    }
    \endqml

    The default interval the date values are chosen is a window starting at
    the current date ending 50 years later. This window is defined by the
    \a minimum and \a maximum properties. The interval can be altered considering
    the following rules:
    \list
        \li - \a minimum must be less or equal than the \l date; if the \a date
                value is less than the given \a minimum, the date will be set to
                the minimum's value
        \li - \a maximum value must be greater than the \a minimum, or invalid.
                When the maximum is smaller than the \l date, the \l date property
                will be updated to get the maximum value.
                When set to invalid date (see DateUtils getInvalidDate()), the
                upper limit of the date interval becomes infinite, meaning the
                year picker will extend infinitelly. This leads to increased
                memory use and should be avoided if possible.
    \endlist
    \qml
    import QtQuick 2.0
    import Ubuntu.Components 0.1
    import Ubuntu.Components.Pickers 0.1

    Column {
        Label {
            text: "Selected date: " + Qt.formatDate(datePicker.date, "dddd, dd-mmmm-yyyy")
        }
        DatePicker {
            id: datePicker
            minimum: {
                var d = new Date();
                d.setFullYear(d.getFullYear() - 1);
                return d;
            }
            maximum: Date.prototype.getInvalidDate.call()
        }
    }
    \endqml
    \b Note: do not use the \l date property when initializing minimum and maximum
    as it will lead in binding loops.

    \section2 Layout
    As mentioned earlier, DatePicker combines up to three Picker tumblers depending
    on the mode requested. These tumblers are laid out in a row in the order the
    default date format of the \l locale is.

    \section3 Date mode layout rules
    The date picker consist of three pickers: year, month, and date. The exact
    contents of the month and date pickers depends on the available width:
    \list
        \li full name for month, number and full day for date (“August” “28 Wednesday”)
        \li otherwise full name for month, number and abbreviated day
            for date (“August” “28 Wed”);
        \li otherwise full name for month, number for date (“August” “28”);
        \li otherwise abbreviated name for month, number for date (“Aug” “28”).
        \li otherwise number for month, number for date (“08” “28”).
    \endlist

    \a{If the currently selected date becomes impossible due to year change (from a
    leap to a non-leap year when the date is set to February 29) or month change
    (e.g. from a month that has 31 days to one that has fewer when the date is
    set to 31), the date reduces automatically to the last day of the month (i.e
    February 28 or 30th day of the month).}

    \section4 How minimum/maximum affects the tumblers

    If minimum and maximum are within the same year, the year picker will be
    insensitive. If minimum and maximum are within the same month, the month picker
    will also be insensitive.

    \section3 Month mode rules
    The month picker consists of two pickers, one for year and one for month picking.
    the exact contents of the pickers depends on the available width:
    \list
        \li full month name,
        \li otherwise short month name,
        \li otherwise month number.
    \endlist
  */
StyledItem {
    id: datePicker

    /*!
      Specifies what kind of date value selectors should be shown by the picker.
      This is a string of 'flags' separated by '|' separator, where flags are:
      \list
        \li - \b Year (or simply \b Y) - when the year value selector is required
        \li - \b Month (or simply \b M) - when the month value selector is required
        \li - \b Day (or simply \b D) - when the day value selector is required
      \endlist
      Any combination of these is allowed, except 'Year|Day'.
      The default value is "Y|M|D".
      */
    property string mode: "Y|M|D"

    /*!
      The date chosen by the DatePicker. The default value is the date at the
      component creation time. The property automatically updates year, month
      and day properties.
      */
    property date date: Date.prototype.midnight.call(new Date())

    /*!
      \qmlproperty int minimum
      \qmlproperty int maximum
      The minimum and maximum dates (inclusive) to be shown in the picker.
      Both year and month values will be considered from the properties.

      The year and month picker values are filled based on these values. The
      year picker will be infinite (extending infinitely) if the maximum is
      an invalid date. If the distance between maximum and minimum is less than
      a year, the year picker will be shown disabled.

      The month picker will be circular if the distance between maximum and minimum
      is at least one year, or if the maximum date is invalid.

      The default values are the current date for the minimum, and 50 year distance
      value for maximum.
      */
    property date minimum: Date.prototype.midnight.call(new Date())
    /*! \internal */
    property date maximum: {
        var d = Date.prototype.midnight.call(new Date());
        d.setFullYear(d.getFullYear() + 50);
        return d;
    }

    /*!
      \qmlproperty int year
      \readonly
      \qmlproperty int month
      \readonly
      \qmlproperty int day
      \readonly
      \qmlproperty int week
      \readonly
      Properties declared for convenience, representing the \b year, \b month, \b day
      and \b week number values of the \l date property.
      */
    readonly property int year: datePicker.date.getFullYear()
    /*! \internal */
    readonly property int month: datePicker.date.getMonth()
    /*! \internal */
    readonly property int day: datePicker.date.getDate()
    /*! \internal */
    readonly property int week: datePicker.date.getWeek()

    /*!
      The property defines the locale used in the picker. The default value is
      the system locale.
      \qml
      DatePicker {
           locale: Qt.locale("hu_HU")
      }
      \endqml
      */
    property var locale: Qt.locale()

    implicitWidth: units.gu(36)
    implicitHeight: {
        var h = Qt.inputMethod.keyboardRectangle.height;
        return (h > 0) ? h : units.gu(20);
    }

    /*! \internal */
    onMinimumChanged: {
        if (internals.completed && !minimum.isValid()) {
            // set the minimum to the date
            minimum = date;
        }

        // adjust date
        if (date !== undefined && date < minimum && minimum.isValid() && internals.completed) {
            date = minimum;
        }
    }
    /*! \internal */
    onMaximumChanged: {
        // adjust date
        if (date !== undefined && date > maximum && maximum.isValid() && maximum > minimum  && internals.completed) {
            date = maximum;
        }
    }
    /*! \internal */
    onWidthChanged: {
        // use dayPicker narrowFormatLimit even if the dayPicker is hidden
        // and clamp the width so it cannot have less width that the sum of
        // the three tumblers' narrowFormatLimit
        var minWidth = 0.0;
        for (var i = 0; i < tumblerModel.count; i++) {
            minWidth += tumblerModel.get(i).pickerModel.narrowFormatLimit;
        }
        width = Math.max(width, minWidth);
    }
    /*! \internal */
    onModeChanged: internals.updatePickers()
    /*! \internal */
    onLocaleChanged: internals.updatePickers()

    Component.onCompleted: {
        if (minimum === undefined) {
            minimum = date;
        }
        internals.completed = true;
        internals.updatePickers();
    }

    // models
    YearModel {
        id: yearModel
        mainComponent: datePicker
        pickerCompleted: internals.completed
        pickerWidth: (!pickerItem) ? 0 : narrowFormatLimit
        function syncModels() {
            dayModel.syncModels();
        }
    }
    MonthModel {
        id: monthModel
        mainComponent: datePicker
        pickerCompleted: internals.completed
        pickerWidth: {
            if (!pickerItem) {
                return 0;
            }
            return MathUtils.clamp(datePicker.width - yearModel.pickerWidth - dayModel.pickerWidth, narrowFormatLimit, longFormatLimit);
        }
        function syncModels() {
            dayModel.syncModels();
        }
    }
    DayModel {
        id: dayModel
        mainComponent: datePicker
        pickerCompleted: internals.completed
        pickerWidth: {
            if (!pickerItem) {
                return 0;
            }
            var w = Math.max((datePicker.width - yearModel.pickerWidth) * internals.dayPickerRatio, narrowFormatLimit);
            if (w < longFormatLimit && w >= shortFormatLimit) {
                return shortFormatLimit;
            }
            return w;
        }
    }

    style: Theme.createStyleComponent("DatePickerStyle.qml", datePicker)
    Binding {
        target: __styleInstance
        property: "view"
        value: positioner
    }
    Binding {
        target: __styleInstance
        property: "pickerModels"
        value: tumblerModel
    }

    // tumbler positioner
    PickerRow {
        id: positioner
        parent: (datePicker.__styleInstance && datePicker.__styleInstance.hasOwnProperty("tumblerHolder")) ?
                    datePicker.__styleInstance.tumblerHolder : datePicker
        mainComponent: datePicker
        model: tumblerModel
        margins: internals.margin
        anchors {
            top: parent.top
            bottom: parent.bottom
            horizontalCenter: parent.horizontalCenter
        }
    }
    // tumbler model
    ListModel {
        /*
              Model to hold tumbler order for repeaters.
              Roles:
              - pickerModel
              - pickerName
              */
        id: tumblerModel

        // the function checks whether a pickerModel was added or not
        // returns the index of the model object the pickerModel was found
        // or -1 on error.
        function pickerModelIndex(name) {
            for (var i = 0; i < count; i++) {
                if (get(i).pickerName === name) {
                    return i;
                }
            }
            return -1;
        }

        // the function checks whether a pickerModel is present in the list;
        // moves the existing one to the given index or inserts it if not present
        function setPickerModel(model, name, index) {
            var idx = pickerModelIndex(name);
            if (idx >= 0) {
                move(idx, index, 1);
            } else {
                append({"pickerModel": model, "pickerName": name});
            }
        }

        // removes the given picker
        function removePicker(name) {
            var idx = pickerModelIndex(name);
            if (idx >= 0) {
                remove(idx);
            }
        }
    }

    // component to calculate text fitting
    Label { id: textSizer; visible: false }
    QtObject {
        id: internals
        property bool completed: false
        property real margin: units.gu(1.5)
        property real dayPickerRatio: 0.1

        property bool showYearPicker: true
        property bool showMonthPicker: true
        property bool showDayPicker: true

        /*
          Update pickers.
          */
        function updatePickers() {
            if (completed) {
                // check mode flags first
                var modes = datePicker.mode.split(/\W/g);
                showYearPicker = (modes.indexOf('Y') >= 0) || (modes.indexOf("Year") >= 0);
                showMonthPicker = (modes.indexOf('M') >= 0) || (modes.indexOf("Month") >= 0);
                showDayPicker = (modes.indexOf('D') >= 0) || (modes.indexOf("Day") >= 0);
                if (!showMonthPicker && showYearPicker && showDayPicker) {
                    console.error("Invalid DatePicker mode: " + datePicker.mode);
                    return;
                }

                arrangeTumblers();
                resetPickers();
            }
        }

        /*
          Resets the pickers. Pickers will update their models with the given date,
          minimum and maximum values.
          */
        function resetPickers() {
            if (!completed) return;
            for (var i = 0; i < tumblerModel.count; i++) {
                var pickerItem = tumblerModel.get(i).pickerModel.pickerItem;
                pickerItem.resetPicker();
            }

            // calculate the ratio for the dayPicker
            var width = datePicker.width - yearModel.pickerWidth;
            dayPickerRatio = (dayModel.longFormatLimit / width).toPrecision(3);
        }

        /*
            Detects the tumbler order from the date format of the locale
          */
        function arrangeTumblers() {
            // disable completion so avoid accidental date changes
            completed = false;

            // use short format to exclude any extra characters
            var format = datePicker.locale.dateFormat(Locale.ShortFormat).split(/\W/g);
            // loop through the format to decide the position of the tumbler
            var formatIndex = 0;
            for (var i in format) {
                if (!format[i].length) continue;
                // check the first two characters
                switch (format[i].substr(0, 1).toLowerCase()) {
                case 'y':
                    if (showYearPicker) {
                        tumblerModel.setPickerModel(yearModel, "YearPicker", formatIndex);
                        formatIndex++;
                    } else {
                        tumblerModel.removePicker("YearPicker");
                    }

                    break;
                case 'm':
                    if (showMonthPicker) {
                        tumblerModel.setPickerModel(monthModel, "MonthPicker", formatIndex);
                        formatIndex++;
                    } else {
                        tumblerModel.removePicker("MonthPicker");
                    }

                    break;
                case 'd':
                    if (showDayPicker) {
                        tumblerModel.setPickerModel(dayModel, "DayPicker", formatIndex);
                        formatIndex++;
                    } else {
                        tumblerModel.removePicker("DayPicker");
                    }
                    break;
                }
            }
            // re-enable completion
            completed = true;
        }
    }
}



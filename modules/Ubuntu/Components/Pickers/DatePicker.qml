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

/*!
    \qmltype DatePicker
    \inqmlmodule Ubuntu.Components.Pickers 0.1
    \ingroup ubuntu-pickers
    \brief DatePicker component provides full date, month or week number picking
    functionality.

    DatePicker combines up to three Picker elements providing different date value
    selection possibilities. It can be used to select full date (year, month, day
    and weekNumber) as well as to select year and month or year and week number
    only. The selected date as well as the initial one is provided by the \l date
    property. For convenience the component provides also the year, month and day
    values as separate properties, however these properties are not writable, and
    the initialization can happen only through the \l date property.
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
        }
    }
    \endqml

    The \l mode property specifies whether the DatePicker provides full date
    picking, month picking or week number picking. When \a Month or \a Week modes
    are set, the component shows only two pickers, one for year and one for the
    month or week picking.
    Month picker example:
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
            mode: "Month"
        }
    }
    \endqml
    Week picker example:
    \qml
    import QtQuick 2.0
    import Ubuntu.Components 0.1
    import Ubuntu.Components.Pickers 0.1

    Column {
        Label {
            text: "Selected week: " + datePicker.week
        }
        DatePicker {
            id: datePicker
            mode: "Week"
        }
    }
    \endqml

    The default behavior of the year picker in the DatePicker is to list the years
    infinitely starting from the year given in the \l date property. This can be
    changed through the \a minimumYear and \a maximumYear properties. In case the
    \a maximumYear value is greater than the \a minimumYear, the year picker will
    no longer be infinite.

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
            minimumYear: new Date().getFullYear() - 1
            maximumYear: new Date().getFullYear() + 1
        }
    }
    \endqml
    \b Note: do not use the \l date property when initializing minimumYear and maximumYear
    as it will cause binding loop.

    \section2 Layout
    As mentioned earlier, DatePicker combines up to three Picker tumblers depending
    on the mode requested. These tumblers will be placed in a row, laid out using
    different rules.

    \section3 Date mode rules
    The date picker consist of three pickers: year, month, and date. The exact
    contents of the month and date pickers depends on the available width:
    \list A
        \li number and full name for month, number and full day for date (“08
            August” “28 Wednesday”)
        \li otherwise number and full name for month, number and abbreviated day
            for date (“08 August” “28 Wed”);
        \li otherwise full name for month, number and abbreviated day for date
            (“August” “28 Wed”);
        \li otherwise full name for month, number for date (“August” “28”);
        \li otherwise abbreviated name for month, number for date (“Aug” “28”).
    \endlist

    \a{If the currently selected date becomes impossible the year change (from a
    leap to a non-leap year when the date is set to February 29) or the month
    (e.g. from a month that has 31 days to one that has fewer when the date is
    set to 31), the date reduces automatically, but should immediately return
    to its previous value if that becomes possible again before you next manually
    change the date.}

    \b Minimum/maximum:

    \list
        \li If minimum and maximum are within the same year, the year picker
            will be insensitive.
        \li If minimum and maximum are within the same month, the month picker
            will be present
    \endlist
    \section3 Month mode rules
    \section3 Week mode rules

    \section2 Styling
    The component's default height is the same asthe on-screen input's height. If
    the environment does not have on-screen input, the height will be set to 20GUs.
    The width is the full width of the phone, 40 GUs.

    The Picker element style used by the DatePicker can be changed through the \l
    pickerStyle property. The Pickers will take the default style as those were
    defined by the current theme.
  */
Rectangle {
    id: datePicker

    /*!
      Specifies the picker mode, whether it should be used for date ("Date"),
      month ("Month") or week ("Week") picking.
      The default value is "Date".
      */
    property string mode: "Date"

    /*!
      The date chosen by the DatePicker. The default value is the date at the
      component creation time. The property automatically updates year, month
      and day properties.
      */
    property date date: new Date()

    /*!
      \qmlproperty int minimumYear
      \qmlproperty int maximumYear
      The minimum and maximum year values (inclusive) to be shown in year picker.
      The \a minimumYear value must be smaller or equal with the \l year value,
      otherwise will be ignored, and the year picker will start from the year
      given in \l date.
      The \a maximumYear must be greater than the minimumValue, or zero (0). If
      set to zero, the year picker will increase the year values infinitely.

      The default values for both is 0.
      */
    property int minimumYear: 0
    /*! \internal */
    property int maximumYear: 0

    /*!
      \qmlproperty int year
      \readonly
      \qmlproperty int month
      \readonly
      \qmlproperty int day
      \readonly
      \qmlproperty int week
      \readonly
      Properties declared for convenience, representing the \b year, \b month and \b day
      values of the \l date property.
      The \b week property is valid only if the \l mode is set to \a Week.
      */
    readonly property int year: datePicker.date.getFullYear()
    /*! \internal */
    readonly property int month: datePicker.date.getMonth()
    /*! \internal */
    readonly property int day: datePicker.date.getDate()
    /*! \internal */
    readonly property int week: DateUtils.weekNumber(datePicker.date)

    /*!
      Specifies whether the date update is live. By default the date is not
      updated live.
      */
    property bool live: false

    /*!
      Property to override the style of the Pickers
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

    // tumblers
    Row {
        anchors.fill: parent
        Picker {
            id: yearPicker
            property bool autoExtending: internals.toYear <= 0
            circular: false
            live: datePicker.live
            model: ListModel{}
            width: internals.minimumWidth
            delegate: PickerDelegate {
                Label {
                    text: modelData
                    anchors {
                        verticalCenter: parent.verticalCenter
                        horizontalCenter: parent.horizontalCenter
                    }
                }

                Component.onCompleted: {
                    if (yearPicker.autoExtending && (index === (yearPicker.model.count - 1))) {
                        internals.extendYearModel(modelData + 1);
                    }
                }
            }
            Binding {
                target: parent
                property: "style"
                value: datePicker.pickerStyle
                when: (datePicker.pickerStyle !== null)
            }
        }
        Picker {
            id: monthPicker
            model: ListModel{}
            width: (parent.width - yearPicker.width - dayPicker.width) > internals.minimumWidth ?
                       (parent.width - yearPicker.width - dayPicker.width) : internals.minimumWidth
            live: datePicker.live
            delegate: PickerDelegate {
                Label {
                    text: {
                        if (datePicker.mode === "Week")
                            return DateUtils.weekText(date, modelData, limits.monthPickerFormat());
                        return DateUtils.monthText(date, modelData, limits.monthPickerFormat());
                    }

                    anchors{
                        verticalCenter: parent.verticalCenter
                        left: parent.left
                        leftMargin: units.gu(0.5)
                    }
                }
            }
            Binding {
                target: parent
                property: "style"
                value: datePicker.pickerStyle
                when: (datePicker.pickerStyle !== null)
            }
        }
        Picker {
            id: dayPicker
            visible: (datePicker.mode === "Date")
            property int widthIndex: limits.dayPickerWidthIndex()
            width: limits.dayPickerWidth(widthIndex)
            live: datePicker.live
            model: ListModel{}
            delegate: PickerDelegate {
                Label {
                    text: DateUtils.dayText(date, modelData + 1, limits.dayPickerFormat(dayPicker.widthIndex))
                    anchors{
                        verticalCenter: parent.verticalCenter
                        left: parent.left
                        leftMargin: units.gu(0.5)
                    }
                }
            }
            Binding {
                target: parent
                property: "style"
                value: datePicker.pickerStyle
                when: (datePicker.pickerStyle !== null)
            }
        }
    }

    /*! \internal */
    onMinimumYearChanged: internals.updateYearModel()
    /*! \internal */
    onMaximumYearChanged: internals.updateYearModel()
    /*! \internal */
    onWidthChanged: {
        // clamp to 3 (or 2) times the minimum Picker width
        width = Math.max(width, (2 + ((mode !== "Date") ? 1 : 0)) * internals.minimumWidth);
    }
    /*! \internal */
    onModeChanged: internals.updateModels();

    Component.onCompleted: {
        internals.completed = true;
        internals.updateModels();
        limits.reset();
    }

    // component to calculate text fitting
    Label { id: textSizer; visible: false }
    ListModel {
        id: limits

        function reset() {
            textSizer.text = "9999"
            internals.minimumWidth = textSizer.paintedWidth + internals.margins;

            limits.clear();
            // greater then
            if (datePicker.mode !== "Week") {
                limits.addElement("99 Wednesday", "long", "99 September", "long");
                limits.addElement("99 Wed", "short", "99 September", "long");
                limits.addElement("99 Wed", "short", "September", "short");
                limits.addElement("99", "narrow", "September", "short");
                limits.addElement("99", "narrow", "Sep", "narrow");
            } else {
                limits.addElement("", "", "W99 September 99 - October 9", "long");
                limits.addElement("", "", "W99 Sep 99 - Oct 9", "short");
                limits.addElement("", "", "W99 Sep 99", "narrow");
            }
        }

        function addElement(dayText, dayFormat, monthText, monthFormat) {
            textSizer.text = dayText;
            var dayWidth = Math.max(textSizer.paintedWidth + internals.margins, internals.minimumWidth);
            textSizer.text = monthText;
            var monthWidth = Math.max(textSizer.paintedWidth + internals.margins, internals.minimumWidth);
            append({"dayWidth": dayWidth,
                    "dayFormat": dayFormat,
                    "monthWidth": monthWidth,
                    "monthFormat": monthFormat
                   });
        }

        function dayPickerWidthIndex() {
            var w = (datePicker.width - yearPicker.width) * 40 / 100;
            for (var i = 0; i < count; i++) {
                if (w >= get(i).dayWidth) {
                    return i;
                }
            }
            return count - 1;
        }
        function dayPickerWidth(index) {
            if (!dayPicker.visible) {
                return 0;
            }

            if (index < 0) {
                return (datePicker.width - yearPicker.width) * 40 / 100;
            }
            return get(index).dayWidth;
        }
        function dayPickerFormat(index) {
            if (!count) return "narrow";
            return (index < 0) ? get(count - 1).dayFormat : get(index).dayFormat;

        }

        function monthPickerFormat() {
            for (var i = 0; i < count; i++) {
                if (monthPicker.width >= get(i).monthWidth) {
                    return get(i).monthFormat;
                }
            }
            return "";
        }
    }

    QtObject {
        id: internals
        property bool completed: false
        property int fromYear
        property int toYear
        property int yearIndex: yearPicker.selectedIndex
        property int monthIndex: monthPicker.selectedIndex
        property int dayIndex: dayPicker.selectedIndex
        property real margins: units.gu(1.5)

        property int minimumWidth

        onYearIndexChanged: {
            if (!completed) return;
            datePicker.date = DateUtils.updateYear(datePicker.date, fromYear + yearPicker.selectedIndex);
            updateDayModel();
        }
        onMonthIndexChanged: {
            if (!completed) return;
            if (datePicker.mode === "Week") {
                // update date to the first day of the week
                var newDate = DateUtils.midDateOfWeek(datePicker.date.getFullYear(), monthPicker.selectedIndex + 1);
                if (fromYear > newDate.getFullYear()) {
                    yearPicker.model.insert(0, {"year": newDate.getFullYear()});
                    // move the year to the previous one
                    yearPicker.selectedIndex--;
                }

                datePicker.date = newDate;
            } else {
                datePicker.date = DateUtils.updateMonth(datePicker.date, monthPicker.selectedIndex)
                updateDayModel();
            }
        }
        onDayIndexChanged: {
            if (!completed || !dayPicker.visible) return;
            datePicker.date = DateUtils.updateDay(datePicker.date, dayPicker.selectedIndex + 1);
        }

        function updateModels() {
            print(1 + datePicker.mode)
            if (!completed) return;
            // turn off completion for awhile
            completed = false;
            yearPicker.model.clear();
            monthPicker.model.clear();
            dayPicker.model.clear();

            updateYearModel();
            updateMidPicker();
            updateDayModel();

            completed = true;
        }

        function extendYearModel(baseData, count) {
            if (count === undefined) {
                count = baseData + 50;
            }

            for (var i = baseData; i <= count; i++) {
                yearPicker.model.append({"year": i});
            }
        }

        function updateYearModel() {
            fromYear = (minimumYear <= 0) ? datePicker.date.getFullYear() : minimumYear;
            toYear = (maximumYear < minimumYear) ? 0 : maximumYear;

            yearPicker.model.clear();
            var max = (toYear >= fromYear) ? toYear : fromYear + 50;
            // check if the year from date is between min..max
            if ((year < fromYear) && (year > max)) {
                date = DateUtils.updateYear(date, fromYear);
            }
            extendYearModel(fromYear, max);
            yearPicker.selectedIndex = year - fromYear;
        }

        function updateMidPicker() {
            if (datePicker.mode === "Week") {
                for (var i = 0; i < 52; i++) {
                    monthPicker.model.append({"week": i});
                }
                monthPicker.selectedIndex = (DateUtils.weekNumber(datePicker.date) - 1);
            } else {
                for (var i = 0; i < 12; i++) {
                    monthPicker.model.append({"month": i});
                }
                monthPicker.selectedIndex = month;
            }
        }

        function updateDayModel() {
            if (!dayPicker.visible) return;
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
    }
}



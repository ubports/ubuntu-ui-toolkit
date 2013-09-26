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
    \brief DatePicker component is used for full date or month picking purposes.

    DatePicker combines up to three Picker elements providing date selection
    possibilities. The component can be used to select full date as well as to
    select year and month only. The selected date as well as the initial one
    is provided by the \l date property. For convenience the component provides
    also the year, month and day values as separate properties.
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

    The component can be used to select the month only, in which case the day
    picker can be hidden by setting false to \l showDays. In this case the day
    property may not provide a valid day value.
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
            showDays: false
        }
    }
    \endqml

    The default behavior of the year picker in the DatePicker is to list the years
    infinitely starting from the year given in the \l date property. This can be
    changed through the \l minimumYear and \l maximumYear properties. The minimumYear
    property is set to the year from the date, and can get less or equal value to
    the year value. Setting a greater or equal value to maximumYear as the minimumYear
    will make year selection to be possible from a range only.
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

    \section3 Styling
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
      otherwise will be ignored.
      The \a maximumYear must be greater than the minimumValue, or zero (0). If
      set to zero, the year picker will increase the year values infinitely.

      The default values are \l year for minimumYear and 0 for maximumYear.
      */
    property int minimumYear: date.getFullYear()
    /*! \internal */
    property int maximumYear: 0

    /*!
      \qmlproperty bool showDays
      The property is used to hide the day picker from the element. The default
      value is true. When set to false, the day property value may be undefined
      and should not be taken into account.
      */
    property alias showDays: dayPicker.visible

    /*!
      \qmlproperty int year
      \readonly
      \qmlproperty int month
      \readonly
      \qmlproperty int day
      \readonly
      Properties declared for convenience, representing the year, month and day
      values of the \l date property.
      */
    readonly property int year: datePicker.date.getFullYear()
    /*! \internal */
    readonly property int month: datePicker.date.getMonth()
    /*! \internal */
    readonly property int day: datePicker.date.getDate()

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
            model: [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11]
            width: (parent.width - yearPicker.width - dayPicker.width) > internals.minimumWidth ?
                       (parent.width - yearPicker.width - dayPicker.width) : internals.minimumWidth
            live: datePicker.live
            delegate: PickerDelegate {
                Label {
                    text: DateUtils.monthText(date, modelData, limits.monthPickerFormat())
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
        // clamp to 3 times the minimum Picker width
        width = Math.max(width, (2 + (showDays == true)) * internals.minimumWidth);
    }

    Component.onCompleted: internals.updateModels();

    // component to calculate text fitting
    Label { id: textSizer; visible: false }
    ListModel {
        id: limits

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
            print(1)
            datePicker.date = DateUtils.updateYear(datePicker.date, fromYear + yearPicker.selectedIndex);
            print(2)
            updateDayModel();
            print(3)
        }
        onMonthIndexChanged: {
            if (!completed) return;
            datePicker.date = DateUtils.updateMonth(datePicker.date, monthPicker.selectedIndex)
            updateDayModel();
        }
        onDayIndexChanged: {
            if (!completed) return;
            datePicker.date = DateUtils.updateDay(datePicker.date, dayPicker.selectedIndex + 1);
        }

        function updateModels() {
            updateYearModel();
            monthPicker.selectedIndex = month;
            updateDayModel();

            textSizer.text = "9999"
            minimumWidth = textSizer.paintedWidth + margins;

            // greater then
            limits.addElement("99 Wednesday", "long", "99 September", "long");
            limits.addElement("99 Wed", "short", "99 September", "long");
            limits.addElement("99 Wed", "short", "September", "short");
            limits.addElement("99", "narrow", "September", "short");
            limits.addElement("99", "narrow", "Sep", "narrow");

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
            print("a");
            fromYear = (minimumYear <= 0) ? datePicker.date.getFullYear() : minimumYear;
            toYear = (maximumYear < minimumYear) ? 0 : maximumYear;

            if (yearPicker.model.count > 0) {
                yearPicker.model.clear();
            }
            var max = (toYear >= fromYear) ? toYear : fromYear + 50;
            // check if the year from date is between min..max
            if ((year < fromYear) && (year > max)) {
                date = DateUtils.updateYear(date, fromYear);
            }
            extendYearModel(fromYear, max);
            yearPicker.selectedIndex = year - fromYear;
        }

        function updateDayModel() {
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



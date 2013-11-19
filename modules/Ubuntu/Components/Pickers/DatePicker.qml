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
  */
StyledItem {
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
      \qmlproperty int minimum
      \qmlproperty int maximum
      The minimum and maximum dates (inclusive) to be shown in the picker.
      Both year and month values will be considered from the properties.

      The year and month picker values are filled based on these values. The
      year picker will be infinite (extending infinitely) if the maximum has
      zero year value set. If the distance between maximum and minimum is zero
      years, the year picker will be shown disabled.

      The mid picker (for month and week value picking) will be circular if the
      distance between maximum and minimum is at least one year, or if the maximum
      date value is zero.

      The default values are the current date for the minimum, and zero value
      for maximum.
      */
    property date minimum: new Date()
    /*! \internal */
    property date maximum: new Date(-1, -1)

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
      Specifies whether the date update is live. By default the date is not
      updated live.
      */
    property bool live: false

    style: Theme.createStyleComponent("DatePickerStyle.qml", datePicker)

    implicitWidth: units.gu(40)
    implicitHeight: {
        var h = Qt.inputMethod.keyboardRectangle.height;
        return (h > 0) ? h : units.gu(20);
    }


    /*! \internal */
    onMinimumChanged: { internals.updateModels() }
    /*! \internal */
    onMaximumChanged: { internals.updateModels() }
    /*! \internal */
    onWidthChanged: {
        // clamp to 3 (or 2) times the minimum Picker width
        var tumblers = 2 + ((mode === "Date") ? 1 : 0);
        width = Math.max(width, tumblers * internals.minimumWidth);
    }
    /*! \internal */
    onModeChanged: internals.updateModels();

    Component.onCompleted: {
        if (minimum === undefined) minimum = date;
        internals.completed = true;
        internals.updateModels();
        internals.initializeLimits();
        print(Qt.formatDate(new Date(), Qt.DefaultLocaleShortDate));
    }

    // tumblers
    Row {
        parent: __styleInstance.hasOwnProperty("contentItem") ? __styleInstance.contentItem : datePicker
        anchors.fill: parent

        DatePickerTemplate {
            id: yearPicker
            picker: datePicker
            completed: internals.completed
            model: YearModel{}
            updatePickerWhenChanged: dayPicker
            width: internals.minimumWidth
        }

        DatePickerTemplate {
            id: monthPicker
            picker: datePicker
            completed: internals.completed
            updatePickerWhenChanged: dayPicker
            model: MonthModel {
                mode: datePicker.mode
            }
            width: Math.max(parent.width - yearPicker.width - dayPicker.width, internals.minimumWidth)
        }

        DatePickerTemplate {
            id: dayPicker
            picker: datePicker
            visible: datePicker.mode !== "Week"
            completed: internals.completed
            model: DayModel{}
            width: (visible && limits && (desiredWidth > 0.0)) ? limits.clampWidth(desiredWidth) : 0
            property real desiredWidth: (datePicker.width - yearPicker.width) * 40 / 100
        }
    }

    // component to calculate text fitting
    Label { id: textSizer; visible: false }
    QtObject {
        id: internals
        property bool completed: false
        property real margin: units.gu(0.5)

        property int minimumWidth
        property int minimumTextWidth

        function updateModels() {
            if (!completed) return;
            // turn off completion for a while
            completed = false;
            yearPicker.resetModel();
            monthPicker.resetModel();
            dayPicker.resetModel();
            completed = true;
        }
        function initializeLimits() {
            textSizer.text = "9999"
            minimumWidth = textSizer.paintedWidth + 2 * margin;
            monthPicker.limits = new DateUtils.PickerLimits(textSizer, (mode == "Week" ? "Weeks" : "Months"), margin, minimumWidth);
            dayPicker.limits = new DateUtils.PickerLimits(textSizer, "Days", margin, minimumWidth);
        }
    }
}



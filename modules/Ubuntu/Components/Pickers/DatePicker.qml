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
    \brief DatePicker component provides full date or month picking functionality.

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
FocusScope {
    id: datePicker

    /*!
      Specifies the picker mode, whether it should be used for date ("Date") or
      month ("Month") picking.
      The default value is "Date".
      */
    property string mode: "Date"

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
      year picker will be infinite (extending infinitely) if the maximum has
      zero year value set. If the distance between maximum and minimum is zero
      years, the year picker will be shown disabled.

      The month picker will be circular if the distance between maximum and minimum
      is at least one year, or if the maximum date value is zero.

      The default values are the current date for the minimum, and zero value
      for maximum.
      */
    property date minimum: Date.prototype.midnight.call(new Date())
    /*! \internal */
    property date maximum: Date.prototype.getInvalidDate.call()

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

    /*
      \qmlproperty Locale locale
      The property defines the locale used in the picker. This can be overridden
      by setting a different Locale object.
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
    onLocaleChanged: internals.resetPickers()
    /*! \internal */
    onMinimumChanged: { internals.resetPickers() }
    /*! \internal */
    onMaximumChanged: { internals.resetPickers() }
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
    onModeChanged: internals.resetPickers();

    Component.onCompleted: {
        if (minimum === undefined) minimum = date;
        internals.arrangeTumblers();
        internals.completed = true;
        internals.resetPickers();
    }

    YearModel {
        id: yearModel
        compositPicker: datePicker
        pickerWidth: Math.max(datePicker.width * 0.24, narrowFormatLimit)
        function syncModels() {
            dayModel.syncModels();
        }
    }
    MonthModel {
        id: monthModel
        compositPicker: datePicker
        pickerWidth: Math.max(datePicker.width - yearModel.pickerWidth - dayModel.pickerWidth, narrowFormatLimit)
        function syncModels() {
            dayModel.syncModels();
        }
    }
    DayModel {
        id: dayModel
        compositPicker: datePicker
        property bool inUse: false
        pickerWidth: {
            if (!inUse) {
                return 0;
            }
            var w = Math.max(datePicker.width * 0.37, narrowFormatLimit);
            if (w < longFormatLimit && w >= shortFormatLimit) {
                return shortFormatLimit;
            }
            return w;
        }
    }

    // tumbler positioner
    StyledItem {
        id: holder
        anchors.fill: parent

        //declare properties that will be used by the PickerStyle
        property alias itemList: positioner
        property Item highlightOverlay

        style: Theme.createStyleComponent("PickerStyle.qml", holder)

        Row {
            id: positioner
            parent: (holder.__styleInstance && holder.__styleInstance.hasOwnProperty("tumblerHolder")) ?
                        holder.__styleInstance.tumblerHolder : holder
            anchors.fill: parent

            Repeater {
                model: ListModel {
                    /*
                      Model to hold tumbler order for repeaters.
                      Roles:
                      - pickerModel
                      */
                    id: tumblerModel
                }
                Picker {
                    id: pickerDelegate
                    model: pickerModel
                    enabled: pickerModel.count > 1
                    circular: pickerModel.circular
                    live: datePicker.live
                    width: pickerModel.pickerWidth

                    style: Rectangle {
                        anchors.fill: parent
                        color: (pickerDelegate.Positioner.index % 2) ? Qt.rgba(0, 0, 0, 0.03) : Qt.rgba(0, 0, 0, 0.07)
                    }
                    delegate: PickerDelegate {
                        Label {
                            text: pickerModel.text(datePicker.date, modelData, pickerModel.pickerWidth);
                            color: Theme.palette.normal.backgroundText
                            anchors.fill: parent
                            verticalAlignment: Text.AlignVCenter
                            horizontalAlignment: Text.AlignHCenter
                        }
                        Component.onCompleted: {
                            if (pickerModel && pickerModel.autoExtend && (index === (pickerModel.count - 1))) {
                                pickerModel.extend(modelData + 1);
                            }
                        }
                    }

                    onSelectedIndexChanged: {
                        if (!internals.completed) return;
                        datePicker.date = pickerModel.dateFromIndex(datePicker.date, selectedIndex);
                        pickerModel.syncModels();
                    }

                    /*
                      Resets the Picker model and updates the new format limits.
                      */
                    function resetPicker() {
                        pickerModel.resetLimits(textSizer, internals.margin);
                        selectedIndex = pickerModel.indexOf(datePicker.date);
                    }

                    Component.onCompleted: {
                        pickerModel.pickerItem = pickerDelegate;
                    }
                }
            }
        }
    }

    // component to calculate text fitting
    Label { id: textSizer; visible: false; scale: 1.2 }
    QtObject {
        id: internals
        property bool completed: false
        property real margin: units.gu(1.5)

        /*
          Resets the pickers. Pickers will update their models with the given date,
          minimum and maximum values.
          */
        function resetPickers() {
            if (!completed) return;
            // turn off completion for a while
            completed = false;
            for (var i = 0; i < tumblerModel.count; i++) {
                var model = tumblerModel.get(i);
                model.pickerModel.reset(datePicker.date, datePicker.minimum, datePicker.maximum);
            }
            completed = true;
        }

        /*
            Detects the tumbler order from the date format of the locale
          */
        function arrangeTumblers() {
            // use short format to exclude any extra characters
            var format = datePicker.locale.dateFormat(Locale.ShortFormat).split(/\W/g);
            // loop through the format to decide the position of the tumbler
            for (var i in format) {
                if (!format[i].length) continue;
                // check the first two characters
                switch (format[i].substr(0, 1).toLowerCase()) {
                case 'y':
                    tumblerModel.append({"pickerModel": yearModel})
                    break;
                case 'm':
                    tumblerModel.append({"pickerModel": monthModel})
                    break;
                case 'd':
                    if (datePicker.mode !== "Month") {
                        dayModel.inUse = true;
                        tumblerModel.append({"pickerModel": dayModel})
                    }
                    break;
                }
            }
        }
    }
}



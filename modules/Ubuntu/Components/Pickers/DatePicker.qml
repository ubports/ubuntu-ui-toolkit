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
    and weekNumber) as well as to select year and month only. The selected date as
    well as the initial one is provided by the \l date property. For convenience
    the component provides also the year, month, day and week values as separate
    properties, however these properties are not writable, and their initialization
    can happen only through the \l date property.
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

    The \l mode property specifies whether the DatePicker provides full date picking
    or month picking. When \a Month mode is set, the component shows only two pickers,
    one for year and one for the month picking.
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

    The default interval the date values are chosen is a window starting at
    the current date ending 50 years later. This window is defined by the
    \a minimum and \a maximum properties. The interval can be altered considering
    the following rules:
    \list
        \li - \a minimum must be less or equal than the \l date;
        \li - \a maximum value must be greater than the \a minimum, or invalid.
                When set to invalid date (see DateUtils getInvalidDate()), the
                upper limit of the date interval mecomes infinite, meaning the
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

    \a{If the currently selected date becomes impossible the year change (from a
    leap to a non-leap year when the date is set to February 29) or the month
    (e.g. from a month that has 31 days to one that has fewer when the date is
    set to 31), the date reduces automatically, but should immediately return
    to its previous value if that becomes possible again before you next manually
    change the date.}

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
      year picker will be infinite (extending infinitely) if the maximum is
      an invalid date. If the distance between maximum and minimum is zero
      years, the year picker will be shown disabled.

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
      The property defines the locale used in the picker. This can be overridden
      by setting a different Locale object.
      \qml
      DatePicker {
           locale: Qt.locale("hu_HU")
      }
      \endqml
      */
    property var locale: Qt.locale()

    /*!
      \internal
      Forwarded style properties to have DatePicker acting as a StyledItem
      */
    property alias style: holder.style
    /*! \internal */
    property alias __styleInstance: holder.__styleInstance

    implicitWidth: units.gu(36)
    implicitHeight: {
        var h = Qt.inputMethod.keyboardRectangle.height;
        return (h > 0) ? h : units.gu(20);
    }

    /*! \internal */
    onMinimumChanged: {
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
        pickerWidth: Math.max(datePicker.width * 0.24, narrowFormatLimit)
        function syncModels() {
            dayModel.syncModels();
        }
    }
    MonthModel {
        id: monthModel
        mainComponent: datePicker
        pickerCompleted: internals.completed
        pickerWidth: Math.max(datePicker.width - yearModel.pickerWidth - dayModel.pickerWidth, narrowFormatLimit)
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
        property alias view: positioner

        style: Theme.createStyleComponent("DatePickerStyle.qml", holder)

        // used in DatePickerStyle to determine the width of the individual tumblers
        property alias pickerModels: tumblerModel

        Row {
            id: positioner
            objectName: "DatePicker_Positioner"
            parent: (holder.__styleInstance && holder.__styleInstance.hasOwnProperty("tumblerHolder")) ?
                        holder.__styleInstance.tumblerHolder : holder
            anchors.fill: parent

            Repeater {
                model: ListModel {
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
                }
                Picker {
                    id: pickerDelegate
                    objectName: "DatePicker_" + pickerName
                    model: pickerModel
                    enabled: pickerModel.count > 1
                    circular: pickerModel.circular
                    live: false
                    width: pickerModel.pickerWidth

                    style: Rectangle {
                        anchors.fill: parent
                        color: (pickerDelegate.Positioner.index % 2) ? Qt.rgba(0, 0, 0, 0.03) : Qt.rgba(0, 0, 0, 0.07)
                    }
                    delegate: PickerDelegate {
                        Label {
                            objectName: "DatePicker_PickerLabel"
                            text: pickerModel.text(modelData)
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
                        if (model && !model.resetting) {
                            datePicker.date = pickerModel.dateFromIndex(selectedIndex);
                            pickerModel.syncModels();
                        }
                    }

                    /*
                      Resets the Picker model and updates the new format limits.
                      */
                    function resetPicker() {
                        model.reset();
                        model.resetLimits(textSizer, internals.margin);
                        model.resetCompleted();
                        selectedIndex = model.indexOf();
                    }

                    Component.onCompleted: {
                        // update model with the item instance
                        model.pickerItem = pickerDelegate;
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
          Update pickers.
          */
        function updatePickers() {
            if (completed) {
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
                    tumblerModel.setPickerModel(yearModel, "YearPicker", formatIndex);
                    formatIndex++;
                    break;
                case 'm':
                    tumblerModel.setPickerModel(monthModel, "MonthPicker", formatIndex);
                    formatIndex++;
                    break;
                case 'd':
                    if (datePicker.mode !== "Month") {
                        tumblerModel.setPickerModel(dayModel, "DayPicker", formatIndex);
                        formatIndex++;
                    } else {
                        var idx = tumblerModel.pickerModelIndex("DayPicker");
                        if (idx >= 0) {
                            tumblerModel.remove(idx, 1);
                        }
                    }
                    break;
                }
            }
            // re-enable completion
            completed = true;
        }
    }
}



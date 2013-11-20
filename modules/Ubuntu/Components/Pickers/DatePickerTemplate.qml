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

/*
  Picker template component for DatePicker
  It is expected from the model used to have the following API:
  ListModel {

      property bool circular: to specify whether the model data is made for circular Picker

      property real narrowFormatLimit:
      property real shortFormatLimit:
      property real longFormatLimit: specifies the text width for the narrow, short and long
                    formats

      [property bool autoExtend: to specify whether the model can extend itself (non-circular
                    model only)]

      function reset(date, minimum, maximum) - reset model with the given date, minimum and
                    maximum date values

      function updateLimits(label, locale) - updates the minimum and maximum width using a
                    Label and locale

      function indexOf(date) - returns the model index of the date

      function dateFromIndex(date, index) - creates a Date object from the model index, relative
                    to the date

      function text(date, value, width, locale) - returns the date string for the value relative
                    to the date, which fits into the given width. Uses the locale to fetch the
                    localized date string.
  }
  */
Picker {
    id: item
    property Item picker
    property bool completed: false
    property var updatePickerWhenChanged: null
    property int pickerIndex: Positioner.index

    style: Theme.createStyleComponent("FlatPickerStyle.qml", item)

    enabled: model.count > 1
    live: picker.live
    circular: model.circular

    delegate: PickerDelegate {
        Label {
            text: item.model.text(picker.date, modelData, item.width, picker.locale);
            color: Theme.palette.normal.backgroundText
            anchors.fill: parent
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
        }
        Component.onCompleted: {
            if (model && model.hasOwnProperty("autoExtend") && model.autoExtend && (index === (model.count - 1))) {
                model.extend(modelData + 1);
            }
        }
    }

    onSelectedIndexChanged: {
        if (!completed || !visible) return;
        picker.date = model.dateFromIndex(picker.date, selectedIndex);
        if (updatePickerWhenChanged) {
            updatePickerWhenChanged.update(picker.date);
        }
    }

    /*
      Resets the Picker model and updates the new format limits.
      */
    function resetModel(label, margin) {
        if (!visible) return;

        model.reset(picker.date, picker.minimum, picker.maximum);
        model.updateLimits(label, margin, picker.locale);
        selectedIndex = model.indexOf(picker.date);
    }
}
